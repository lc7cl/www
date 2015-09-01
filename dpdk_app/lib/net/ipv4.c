#include <rte_ether.h>
#include <rte_ip.h>

#include "af_inet.h"
#include "hook.h"
#include "ipv4.h"

static void ipv4_finish(struct rte_mbuf *mbuf)
{
	struct ipv4_hdr *iphdr;
	struct net_protocol *proto;

	iphdr = (struct ipv4_hdr*) rte_pktmbuf_mtod(mbuf, struct rte_mbuf*);

	/*TODO: defragment*/
	
	rte_pktmbuf_adj(mbuf, (iphdr->version_ihl & IPV4_HDR_IHL_MASK) * IPV4_IHL_MULTIPLIER);
	proto = inet_protos[iphdr->next_proto_id];
	if (proto == NULL)
		goto drop_mbuf;
	proto->handler(mbuf, iphdr);
    return;
	
drop_mbuf:
	TRACE_DROP_MBUF(mbuf, 1);
	rte_pktmbuf_free(mbuf); 
}

void ipv4_rcv(struct rte_mbuf *mbuf, struct packet_type *pt)
{
	struct ipv4_hdr *iphdr;
	
	if (pt->type != ETHER_TYPE_IPv4) {
		goto drop_mbuf;
	}

	iphdr = (struct ipv4_hdr*) rte_pktmbuf_mtod(mbuf, struct rte_mbuf*);
	if ((iphdr->version_ihl >> 4 & IPV4_VERSION_MASK) == 4
		|| (iphdr->version_ihl & IPV4_HDR_IHL_MASK) < 5) {
		goto drop_mbuf;
	}

	if (rte_ipv4_cksum(iphdr) != 0xffff) {
		goto drop_mbuf;
	}

	hook_proccess(mbuf, HOOK_PROTO_IPV4, HOOK_PRE_ROUTING, ipv4_finish);
	return;

drop_mbuf:
	TRACE_DROP_MBUF(mbuf, 1);
	rte_pktmbuf_free(mbuf);	
}

int ip_pkt_build(struct sock *sk, struct rte_mbuf *mb, be32 dst_addr)
{
	struct ipv4_hdr *iphdr;
	//uint16_t tot_len;
	struct net_device *ndev;
	be32 src_addr;
	
	NET_ASSERT(mb != NULL);

	iphdr = (struct ipv4_hdr*)rte_pktmbuf_prepend(mb, sizeof *iphdr);
	if (iphdr == NULL)
		return -1;

	iphdr->version_ihl = 0x45;
	iphdr->type_of_service = 0;
	iphdr->total_length = rte_cpu_to_be_16(rte_pktmbuf_data_len(mb));
	iphdr->packet_id = rte_cpu_to_be_16(sk->next_ip_id++);
	iphdr->fragment_offset = 0;
	iphdr->time_to_live = DEFAULT_TTL;
	iphdr->next_proto_id = sk->l4_proto;
	iphdr->hdr_checksum = 0;
	ndev = net_device_get(sk->nic);
	if (ndev && ndev->v4_addr) {
		src_addr = ndev->v4_addr->addr.ipv4;		
	} else {
		src_addr = 0;
	}
	iphdr->src_addr = rte_cpu_to_be_32(src_addr);
	iphdr->dst_addr = dst_addr;
	iphdr->hdr_checksum = rte_ipv4_cksum(iphdr);
	return 0;
}

int ip_output(struct sock *sk, struct rte_mbuf *mb, be32 daddr)
{
	packet_xmit(sk->nic, mb, daddr);
	return 0;
}
