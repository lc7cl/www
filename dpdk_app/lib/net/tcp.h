#ifndef _TCP_H_
#define _TCP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "af_inet.h"

void tcp_rcv(struct rte_mbuf*, struct ipv4_hdr*);


#ifdef __cplusplus
}
#endif

#endif
