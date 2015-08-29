#include <rte_hash.h>
#include <rte_arp.h>
#include "arp.h"

#define MAX_ARP_NODES 4096

struct rte_hash *arp_table;
struct rte_hash_parameters arp_table_params = {
	.name = "arp_table",
	.key_len = sizeof(unsigned),
	.entries = MAX_ARP_NODES,
	.reserved = 0,
	.hash_func = rte_jhash3,
	.socket_id = 0,
};

static struct rte_mempool *arp_mbuf_mp;

void arp_rcv(struct rte_mbuf *mbuf, __rte_unused struct packet_type *pt)
{
	struct arp_hdr *arp_hdr;
	struct arp_ipv4 *payload;
	struct net_device *ndev;
	uint32_t sip;

	ndev = net_device_get(mbuf->port);
	if (ndev == NULL || ndev->portid != mbuf->port) 
		goto release_mbuf;
	
	arp_hdr = rte_pktmbuf_mtod(mbuf, struct arp_hdr*);
	payload = (struct arp_ipv4*)(arp_hdr + 1);
	
	sip = payload->arp_sip;

	if (arp_hdr->arp_op == ARP_OP_REQUEST) {
		/*proccess duplicate ip detection*/
		if (sip == 0 && net_device_inet_addr_match(ndev, sip)) {
			arp_send(ndev, ARP_OP_REPLY, &ndev->haddr, );
		}
	}
	
release_mbuf:
	rte_pktmbuf_free(mbuf);
	return;
}

int arp_send(struct net_device *ndev, uint16_t op, struct ether_hdr *shaddr, be32 saddr, 
	struct ether_hdr *dhaddr, be32 daddr)
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
	eth_hdr = rte_pktmbuf_mtod(m, struct ether_hdr);
	ether_addr_copy(dhaddr, &eth_hdr->d_addr);
	ether_addr_copy(ndev->portid, &eth_hdr->s_addr);
	eth_hdr->ether_type = rte_cpu_to_be_16(ETHER_TYPE_ARP);

	/*arp header*/
	arp_hdr = (struct arp_hdr *)(eth_hdr + 1);
	arp_hdr->arp_hrd = rte_cpu_to_be_16(ARP_HRD_ETHER);
	arp_hdr->arp_pro = rte_cpu_to_be_16(ETHER_TYPE_IPv4);
	arp_hdr->arp_hln = sizeof (struct ether_addr);
	arp_hdr->arp_pln = sizeof be32;
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

int arp_init(void)
{
	arp_table = rte_hash_create(&arp_table_params);
	if (arp_table == NULL)
		return -1;
	arp_mbuf_mp = rte_mempool_create("arp_mempool",
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
	if (arp_mbuf_mp == NULL) {
		rte_hash_free(arp_table);
		return -1;
	}
	return 0;
}

