#include <rte_hash.h>
#include <rte_jhash.h>
#include <rte_arp.h>
#include <rte_cycles.h>

#include "port_queue_map.h"
#include "netdev.h"
#include "arp.h"

#define MAX_ARP_NODES 4096


struct rte_hash *arp_table;
struct rte_hash_parameters arp_table_params = {
	.name = "arp_table",
	.key_len = sizeof(unsigned),
	.entries = MAX_ARP_NODES,
	.reserved = 0,
	.hash_func = rte_jhash,
	.socket_id = 0,
};

static struct rte_mempool *arp_mbuf_mp;

static struct arp_node* arp_node_create(be32 addr, struct ether_addr *haddr, unsigned state)
{
	struct arp_node *new;

	new = rte_malloc(NULL, sizeof(*new), 0);
	if (!new)
		return NULL;

	new->state = state;
	new->addr = addr;
	if (haddr) {
		/*TODO validicate haddr*/		
		ether_addr_copy(haddr, &new->haddr);
	}
	rte_timer_init(&new->timer);
	TAILQ_INIT(&new->backlog);
	new->update_time = rte_get_tsc_cycles();

	return new;	
}

static int arp_node_update(be32 addr, struct ether_addr *haddr, unsigned state, int create)
{
	struct arp_node *node;

	if (rte_hash_lookup_data(arp_table, &addr, (void**)&node)) {
		if (create) {
			node = arp_node_create(addr, haddr, state);
			if (node == NULL)
				return -1;
			if (rte_hash_add_key_data(arp_table, &addr, node)) {
				rte_free(node);
				return -1;
			}		
			return 0;
		}
	} else {
		ether_addr_copy(haddr, &node->haddr);
		node->state = state;
		node->update_time = rte_get_tsc_cycles();
	}
	return 0;
}

int arp_send(struct net_device *ndev, uint16_t op, struct ether_hdr *shaddr, be32 saddr, 
	const struct ether_hdr *dhaddr, be32 daddr)
{
	struct rte_mbuf *m;
	struct ether_hdr *eth_hdr;
	struct arp_hdr *arp_hdr;
	struct arp_ipv4 *payload;
	struct lcore_queue_conf *lcore_q;
	int qid;

	m = rte_pktmbuf_alloc(arp_mbuf_mp);
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
	payload->sip = saddr;
	ether_addr_copy(dhaddr, &payload->arp_tha);
	payload->dip = daddr;

	lcore_q = lcore_q_conf_get(rte_lcore_id());
	qid = lcore_q->nb_txq[lcore_q->next_txq++].qid;

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
		RTE_LOG(DEBUG, NET, "invalid arp_op\n");
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
			arp_node_update(payload->arp_sip, payload->arp_sha, ARP_S_COMPELTE, 1);			
		}
		goto release_mbuf;		
	}

	if (arp_hdr->arp_op == ARP_OP_REPLY) {
		struct arp_node *node;

		if (rte_hash_lookup_data(arp_table, &payload->arp_sip, &node))
			arp_node_update(payload->arp_sip, payload->arp_sha, ARP_S_STALE, 1);
		else
			arp_node_update(payload->arp_sip, payload->arp_sha, ARP_S_COMPELTE, 0);		
	} 
	
release_mbuf:
	rte_pktmbuf_free(mbuf);
	return;
}

int arp_init(void)
{
	arp_table = rte_hash_create(&arp_table_params);
	if (arp_table == NULL)
		return -1;
	arp_mbuf_mp = rte_mempool_create("arp_mempool",
		MAX_ARP_NODES,
		sizeof(struct arp_node),
		0,
		0,
		NULL,
		NULL,
		NULL,
		NULL,
		0,
		0);
	if (arp_mbuf_mp == NULL) {
		rte_hash_free(arp_table);
		return -1;
	}
	return 0;
}

