#ifndef _BUFFER_H_
#define _BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <rte_mbuf.h>

struct net_device;

enum mbuf_phase {
	MBUF_P_NONE = 0,
	MBUF_P_ARP,
	MBUF_P_IPV4,
	MBUF_P_TCP,
	MBUF_P_UDP,
	MBUF_P_MAX = 255
};

#define MBUF_P_MAX 255

#ifdef TRACE_MBUF
static inline void trace_mbuf(struct rte_mbuf *mbuf)
{
	
}
#else
static inline void trace_mbuf(struct rte_mbuf *mbuf) {}
#endif

void stat_mbuf(struct rte_mbuf *mbuf, uint8_t in, uint8_t drop, uint8_t phase);

#define TRACE_DROP_MBUF(m, in, p) do {	\
	stat_mbuf(m, 1, in, p);				\
	trace_mbuf(m);						\
} while(0)

#ifdef __cplusplus
}
#endif

#endif
