#include <rte_hash.h>
#include <rte_jhash.h>

#include "buffer.h"
#include "sk.h"
#include "udp.h"

static struct rte_hash *uhtable;

struct udp_key {
	uint32_t dst_addr;		
	uint16_t dst_port;
};

static struct rte_hash_parameters uhash_param = {
	.name = "udp_hash",
	.entries = 256,
	.key_len = sizeof(struct udp_key),
	.hash_func = rte_jhash,
};

void udp_rcv(struct rte_mbuf *mbuf, struct ipv4_hdr *ipv4_hdr)
{
	struct udp_hdr *udp_hdr;
	struct sk *s = NULL;
	struct udp_key ukey;

	if (mbuf)
		return;

	udp_hdr = rte_pktmbuf_mtod(mbuf, struct udp_hdr*);
	if (rte_ipv4_udptcp_cksum(ipv4_hdr, udp_hdr) != 0xffff)
		goto drop;

	memset(&ukey, 0, sizeof ukey);
	ukey.dst_addr = ipv4_hdr->dst_addr;
	ukey.dst_port = udp_hdr->dst_port;
	
	rte_hash_lookup_data(uhtable, (const void*)&ukey, (void **)&s);
	if (s == NULL)
		goto drop;
    if (s->param.mode == SOCK_MODE_COMPLETE)
	    s->param.func(mbuf);

drop:
	rte_pktmbuf_free(mbuf);
}

int udp_init(void)
{
	uhtable = rte_hash_create(&uhash_param);
	if (uhtable == NULL)
		return -1;
	
	return 0;
}


int udp_send(__rte_unused struct sock *sk, __rte_unused char *buff, __rte_unused int length);
int udp_send(__rte_unused struct sock *sk, __rte_unused char *buff, __rte_unused int length)
{
	return 0;
}

int udp_bind(struct sock *sk, uint32_t addr, uint16_t port);
int udp_bind(struct sock *sk, uint32_t addr, uint16_t port) 
{
	struct udp_key ukey;

	ukey.dst_addr = addr;
	ukey.dst_port = port;
	return rte_hash_add_key_data(uhtable, &ukey, sk);
}

struct proto udp_proto = {
	.send = &udp_send,
	.bind = &udp_bind,
};
