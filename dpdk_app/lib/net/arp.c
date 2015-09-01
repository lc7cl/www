#include <rte_hash.h>
#include <rte_jhash.h>
#include <rte_arp.h>
#include <rte_cycles.h>
#include <rte_ethdev.h>

#include "port_queue_map.h"
#include "netdev.h"
#include "arp.h"

#define MAX_ARP_NODES 4096
#define ARP_PROBE_PERIOD (10 * 1000 * 1000) /*10s*/
#define ARP_AGEING_PERIOD (20 * 1000 * 1000)/*20s*/

struct rte_hash_parameters arp_table_params = {
	.name = "arp_table",
	.key_len = sizeof(unsigned),
	.entries = MAX_ARP_NODES,
	.reserved = 0,
	.hash_func = rte_jhash,
	.socket_id = 0,
};

static int add_ether_hdr_and_send(struct arp_node *node, struct rte_mbuf *mbuf)
{
	struct ether_hdr *hdr;

	rte_pktmbuf_prepend(mbuf, sizeof(struct ether_hdr));
	hdr = rte_pktmbuf_mtod(mbuf, struct ether_hdr *);
	ether_addr_copy(&node->haddr, &hdr->d_addr);
	ether_addr_copy(&node->ndev->haddr, &hdr->s_addr);
	hdr->ether_type = rte_cpu_to_be_16(ETHER_TYPE_IPv4);
	return node->ndev->ops->xmit(node->ndev, mbuf);
}

static int sendpkt(struct arp_node *node, struct rte_mbuf *mbuf)
{
	struct pktbuf *pkt;

	pkt = get_pktbuf(mbuf);

	NET_ASSERT(pkt != NULL);
	
	if (node->state == ARP_S_COMPELTE) {
		if (TAILQ_EMPTY(&node->backlog)) {
			/*send direct*/
			return add_ether_hdr_and_send(node, mbuf);
		} else {
			TAILQ_INSERT_TAIL(&node->backlog, pkt, list);
			TAILQ_FOREACH(pkt, &node->backlog, list) {
				if (add_ether_hdr_and_send(node, mbuf)) {
					break;
				}
			}
		}
	} else if (node->state != ARP_S_DISCARD) {
		TAILQ_INSERT_TAIL(&node->backlog, pkt, list);
	} else {
		TRACE_DROP_MBUF(mbuf, 0);
		return 1;
	}
	return 0;
}

static int arp_probe(struct rte_timer *timer, __rte_unused void* arg) 
{
	struct arp_node *node;
	struct net_device *ndev;
	be32 src_addr;

	node = (struct arp_node *)((char*)timer - offsetof(struct arp_node, timer));
	src_addr = net_device_get_primary_addr(node->ndev->portid);
	if (src_addr && 
		arp_send(node->ndev, ARP_OP_REQUEST, &ndev->haddr, src_addr, &node->haddr, node->addr)) {
		node->state = ARP_S_PROBE;
	} else {
		node->state = ARP_S_DISCARD;
		rte_hash_del_key(ndev->arp_table, &node->addr);
		rte_free(node);
	}
	
	return 0;
}

struct arp_node* arp_node_create(struct net_device *ndev, be32 addr, struct ether_addr *haddr, unsigned state)
{
	struct arp_node *new;
	int retval;

	NET_ASSERT(ndev != NULL);

	new = rte_malloc(NULL, sizeof(struct arp_node), 0);
	if (new == NULL)
		return NULL;

	new->ndev = ndev;
	new->state = state;
	new->addr = addr;
	if (haddr) {
		/*TODO validicate haddr*/		
		ether_addr_copy(haddr, &new->haddr);
	}
	rte_timer_init(&new->timer);
	rte_timer_reset(&new->timer, ARP_PROBE_PERIOD, SINGLE, LCORE_ID_ANY, arp_probe, NULL);
	TAILQ_INIT(&new->backlog);
	new->update_time = rte_rdtsc_precise();
	new->sendpkt = sendpkt;

	return new;	
}

static int arp_node_update(struct net_device *ndev, be32 addr, struct ether_addr *haddr, unsigned state, int create)
{
	struct arp_node *node;
	struct pktbuf *pkt;

	if (rte_hash_lookup_data(ndev->arp_table, &addr, (void**)&node)) {
		if (create) {
			node = arp_node_create(ndev, addr, haddr, state);
			if (node == NULL)
				return -1;
			if (rte_hash_add_key_data(ndev->arp_table, &addr, node)) {
				rte_free(node);
				return -1;
			}		
			return 0;
		}
	} else {
		ether_addr_copy(haddr, &node->haddr);
		node->state = state;
		node->update_time = rte_get_tsc_cycles();
		if (state == ARP_S_COMPELTE) {
			if (rte_timer_pending(&node->timer))
				rte_timer_stop(&node->timer);
			rte_timer_reset(&node->timer, ARP_AGEING_PERIOD, SINGLE, LCORE_ID_ANY, arp_probe, NULL);
			TAILQ_FOREACH(pkt, &node->backlog, list) {
				if (add_ether_hdr_and_send(node, pkt->mbuf))
					break;
			}
		}
	}
	return 0;
}

struct arp_node *arp_node_lookup(struct net_device *ndev, be32 addr, int create)
{
	struct arp_node *node;

	if (rte_hash_lookup_data(ndev->arp_table, &addr, &node)) {
		if (create) {
			node = arp_node_create(ndev, addr, NULL, ARP_S_STALE);
		} else {
			return NULL;
		} 
	}

	return node;
}

int arp_send(struct net_device *ndev, uint16_t op, struct ether_addr *shaddr, be32 saddr, 
	struct ether_addr *dhaddr, be32 daddr)
{
	struct rte_mbuf *m;
	struct ether_hdr *eth_hdr;
	struct arp_hdr *arp_hdr;
	struct arp_ipv4 *payload;
	struct lcore_queue_conf *lcore_q;
	int qid;

	m = rte_pktmbuf_alloc(ndev->arp_mbuf_mp);
	if (m == NULL)
		return -1;
	rte_pktmbuf_append(m, sizeof(struct ether_hdr) + sizeof(struct arp_hdr) + sizeof(struct arp_ipv4));
	/*ether header*/
	eth_hdr = rte_pktmbuf_mtod(m, struct ether_hdr*);
	ether_addr_copy(dhaddr, &eth_hdr->d_addr);
	ether_addr_copy(&ndev->haddr, &eth_hdr->s_addr);
	eth_hdr->ether_type = rte_cpu_to_be_16(ETHER_TYPE_ARP);

	/*arp header*/
	arp_hdr = (struct arp_hdr *)(eth_hdr + 1);
	arp_hdr->arp_hrd = rte_cpu_to_be_16(ARP_HRD_ETHER);
	arp_hdr->arp_pro = rte_cpu_to_be_16(ETHER_TYPE_IPv4);
	arp_hdr->arp_hln = sizeof(struct ether_addr);
	arp_hdr->arp_pln = sizeof(be32);
	arp_hdr->arp_op = rte_cpu_to_be_16(op);

	/*arp payload*/
	payload = (struct arp_ipv4*)(arp_hdr + 1);
	ether_addr_copy(shaddr, &payload->arp_sha);
	payload->arp_sip = saddr;
	ether_addr_copy(dhaddr, &payload->arp_tha);
	payload->arp_tip = daddr;

	lcore_q = lcore_q_conf_get(rte_lcore_id());
	qid = lcore_q->txq[lcore_q->next_txq++].qid;
	lcore_q->next_txq = (lcore_q->next_txq + 1) % lcore_q->nb_txq;

	if (rte_eth_tx_burst(ndev->portid, qid, &m, 1) == 0) {
		rte_pktmbuf_free(m);
		return -1;	
	}
	
	return 0;
}

void arp_rcv(struct rte_mbuf *mbuf, __rte_unused struct packet_type *pt)
{
	struct arp_hdr *arp_hdr;
	struct arp_ipv4 *payload;
	struct net_device *ndev;
	uint32_t sip;
	uint16_t arp_op;

	ndev = net_device_get(mbuf->port);
	if (ndev == NULL || ndev->portid != mbuf->port) 
		goto release_mbuf;
	
	arp_hdr = rte_pktmbuf_mtod(mbuf, struct arp_hdr*);
	payload = (struct arp_ipv4*)(arp_hdr + 1);
	
	sip = payload->arp_sip;
	arp_op = rte_be_to_cpu_16(arp_hdr->arp_op);

	if (arp_op != ARP_OP_REQUEST
		|| arp_op != ARP_OP_REPLY) {
		RTE_LOG(DEBUG, PROTO, "invalid arp_op\n");
		goto release_mbuf;
	}

	if (sip == 0) {
		/*proccess duplicate ip detection*/
		if (arp_op == ARP_OP_REQUEST) {
			arp_send(ndev, ARP_OP_REPLY, &ndev->haddr, payload->arp_tip, &payload->arp_sha, payload->arp_sip);
		}		
		goto release_mbuf;
	}

	if (arp_op == ARP_OP_REQUEST) {
		if (net_device_inet_addr_match(ndev, payload->arp_tip)) {
			arp_send(ndev, ARP_OP_REPLY, &ndev->haddr, payload->arp_tip, &payload->arp_sha, payload->arp_sip);
		} else if (payload->arp_tip == payload->arp_sip) { /*arp announce*/
			arp_node_update(ndev, payload->arp_sip, &payload->arp_sha, ARP_S_COMPELTE, 1);			
		}
		goto release_mbuf;		
	}

	if (arp_hdr->arp_op == ARP_OP_REPLY) {
		struct arp_node *node;

		if (rte_hash_lookup_data(arp_table, &payload->arp_sip, (void**)&node)) {
			arp_node_update(payload->arp_sip, &payload->arp_sha, ARP_S_STALE, 1);
		} else {
			arp_node_update(payload->arp_sip, &payload->arp_sha, ARP_S_COMPELTE, 0);		
		}
	} 
	
release_mbuf:
	rte_pktmbuf_free(mbuf);
	return;
}

int arp_init(struct net_device *ndev)
{
	ndev->arp_table = rte_hash_create(&arp_table_params);
	if (ndev->arp_table == NULL)
		return -1;
	ndev->arp_mbuf_mp = rte_mempool_create("arp_mempool",
		MAX_ARP_NODES,
		sizeof(struct rte_mbuf),
		0,
		0,
		NULL,
		NULL,
		NULL,
		NULL,
		0,
		0);
	if (ndev->arp_mbuf_mp == NULL) {
		rte_hash_free(ndev->arp_table);
		return -1;
	}
	return 0;
}

