#ifndef _UDP_H_
#define _UDP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <rte_ip.h>
#include <rte_udp.h>

#include "sk.h"

extern struct proto udp_proto;

void udp_rcv(struct rte_mbuf *mbuf, struct ipv4_hdr *ipv4_hdr);

int udp_init(void);

#ifdef __cplusplus
}
#endif

#endif
