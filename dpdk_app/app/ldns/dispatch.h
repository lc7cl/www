#ifndef _DISPATCH_H_
#define _DISPATCH_H_

#include <rte_mbuf.h>

int dispatch_dns_pkt(struct rte_mbuf *mbuf, uint32_t addr, uint16_t port);

#endif
