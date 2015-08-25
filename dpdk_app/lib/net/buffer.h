#ifndef _BUFFER_H_
#define _BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <rte_mbuf.h>
#include "netdev.h"

struct net_device;

#ifdef TRACE_MBUF
static inline void trace_mbuf(struct rte_mbuf *mbuf)
{
	printf("drop mbuf from port:%u\n", mbuf->port);
}
#else
static inline void trace_mbuf(__rte_unused struct rte_mbuf *mbuf) {}
#endif

void stat_mbuf(struct rte_mbuf *mbuf, uint8_t in, uint8_t drop);

#define TRACE_DROP_MBUF(m, in) do {	\
	stat_mbuf(m, in, 1);				\
	trace_mbuf(m);						\
} while(0)

#ifdef __cplusplus
}
#endif

#endif
