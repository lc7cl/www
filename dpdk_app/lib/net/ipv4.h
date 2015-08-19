#ifndef _IPV4_H_
#define _IPV4_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "packet.h"

void ipv4_rcv(struct rte_mbuf *mbuf, struct packet_type *pt);

#ifdef __cplusplus
}
#endif

#endif
