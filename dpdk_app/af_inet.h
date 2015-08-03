#ifndef _IP_H_
#define _IP_H_

#define MAX_INET_PROTOS 256
#define IPV4_VERSION_MASK 0xf0
#define IPV4_HEADLENGTH_MASK 0x0f
#define UDP_ID              17
#define TCP_ID              6
#define ICMP_ID             1

int ip_init(void);

void ip_rcv(struct rte_mbuf *mbuf, struct packet_type *pt);

#endif