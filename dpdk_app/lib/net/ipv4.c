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

