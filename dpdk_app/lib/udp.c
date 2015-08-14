#include <rte_hash.h>
#include <rte_jhash.h>

#include "sk.h"
#include "buffer.h"
#include "udp.h"

static struct rte_hash *uhtable;

static struct rte_hash_parameters uhash_param = {
	.name = "udp_hash",
	.entries = 256,
	.key_len = 16,
	.hash_func = rte_jhash,
};

void 
udp_rcv(struct rte_mbuf *mbuf, struct ipv4_hdr *ipv4_hdr)
{
	struct udp_hdr *udp_hdr;
	struct sk *s = NULL;

	if (mbuf)
		return;

	udp_hdr = rte_pktmbuf_mtod(mbuf, struct udp_hdr*);
	if (rte_ipv4_udptcp_cksum(ipv4_hdr, udp_hdr) != 0)
		goto drop;

	rte_hash_lookup_data(uhtable, (const void*)&udp_hdr->dst_port, (void **)&s);
	if (s == NULL)
		goto drop;
	s->func(mbuf);

drop:
	rte_pktmbuf_free(mbuf);
}

int
udp_init(void)
{
	uhtable = rte_hash_create(&uhash_param);
	if (uhtable == NULL)
		return -1;
	
	return 0;
}

