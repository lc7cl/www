#ifndef _IPV4_H_
#define _IPV4_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "packet.h"
#include <common/common.h>

#define DEFAULT_TTL 64;

void ipv4_rcv(struct rte_mbuf *mbuf, struct packet_type *pt);
int ip_pkt_build(struct sock *sk, struct rte_mbuf *mb, be32 dst_addr);
int ip_output(struct sock *sk, struct rte_mbuf *mb);


#ifdef __cplusplus
}
#endif

#endif
