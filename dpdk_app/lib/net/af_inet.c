#include <rte_ether.h>
#include <rte_jhash.h>

#include "packet.h"
#include "udp.h"
#include "tcp.h"
#include "hook.h"
#include "af_inet.h"

static struct net_protocol *inet_protos[MAX_INET_PROTOS];

static struct net_protocol udp_protocol = {
	.protocol = UDP_ID,
	.handler = udp_rcv
};

static struct net_protocol tcp_protocol = {
	.protocol = TCP_ID,
	.handler = tcp_rcv
};

void ip_finish(struct rte_mbuf *mbuf);

int inet_add_protocol(struct net_protocol *protocol)
{
	if (protocol == NULL) {
		return -1;
	}

	inet_protos[protocol->protocol] = protocol;

	return 0;
}

int inet_proto_register(void)
{
	int retval = 0;

	return retval;
}

void ip_finish(struct rte_mbuf *mbuf)
{
	struct ipv4_hdr *iphdr;
	struct net_protocol *proto;

	iphdr = (struct ipv4_hdr*) rte_pktmbuf_mtod(mbuf, struct rte_mbuf*);
	rte_pktmbuf_adj(mbuf, (iphdr->version_ihl & IPV4_HDR_IHL_MASK) * IPV4_IHL_MULTIPLIER);
	proto = inet_protos[iphdr->next_proto_id];
	if (proto == NULL)
		goto drop_mbuf;
	proto->handler(mbuf, iphdr);
	
drop_mbuf:
	TRACE_DROP_MBUF(mbuf, 1);
	rte_pktmbuf_free(mbuf); 
}

void ip_rcv(struct rte_mbuf *mbuf, struct packet_type *pt)
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

	hook_proccess(mbuf, HOOK_PROTO_IPV4, HOOK_POS_IN, ip_finish);
	return;

drop_mbuf:
	TRACE_DROP_MBUF(mbuf, 1);
	rte_pktmbuf_free(mbuf);	
}

static struct packet_type ipv4_packet = {
		.type = ETHER_TYPE_IPv4,
		.func = ip_rcv,
		.list = { NULL, NULL },
	};

int inet_init(void)
{
	int retval;

	retval = udp_init();
	
	retval = packet_type_add(&ipv4_packet);
	retval = inet_add_protocol(&udp_protocol);
	retval = inet_add_protocol(&tcp_protocol);
	return retval;
}
