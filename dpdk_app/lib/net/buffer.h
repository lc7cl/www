#ifndef _BUFFER_H_
#define _BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <rte_mbuf.h>
#include "netdev.h"

struct pktbuf {
	TAILQ_ENTRY(pktbuf) list;
};

static struct pktbuf* get_pktbuf(struct rte_mbuf *mbuf) 
{
	return (struct pktbuf*)mbuf->userdata;
}

struct net_device;

#ifdef TRACE_MBUF
static inline void trace_mbuf(struct rte_mbuf *mbuf, const char *func, int line)
{
	printf("%s %d drop mbuf from port:%u\n", func, line, mbuf->port);
}
#else
static inline void trace_mbuf(__rte_unused struct rte_mbuf *mbuf) {}
#endif

void stat_mbuf(struct rte_mbuf *mbuf, uint8_t in, uint8_t drop);

#define TRACE_DROP_MBUF(m, in) do {	\
	stat_mbuf(m, in, 1);				\
	trace_mbuf(m, __func__, __LINE__);						\
} while(0)

#ifdef __cplusplus
}
#endif

#endif
