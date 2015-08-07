#ifndef _UDP_H_
#define _UDP_H_

#include <rte_ip.h>
#include <rte_udp.h>

void udp_rcv(struct rte_mbuf *mbuf, struct ipv4_hdr *ipv4_hdr);

int udp_init(void);



#endif
