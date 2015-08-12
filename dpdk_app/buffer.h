#ifndef _BUFFER_H_
#define _BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <rte_mbuf.h>

enum mbuf_phase {
	MBUF_P_NONE = 0,
	MBUF_P_ARP,
	MBUF_P_IPV4,
	MBUF_P_TCP,
	MBUF_P_UDP,
	MBUF_P_MAX = 255
};

static inline void
stat_mbuf(struct rte_mbuf *mbuf, uint8_t in, uint8_t drop, uint8_t phase)
{
	struct net_device *ndev;

	ndev = net_device_get(mbuf->port);
	if (ndev->flag & NET_DEV_F_DISABLE == NET_DEV_F_DISABLE)
		return;

	if (in) {
		if (drop) {
			ndev->stat.rx.drop[rte_lcore_id()][phase]++;
		} else {
			ndev->stat.rx.recv[rte_lcore_id()][phase]++;
		}
	} else {
		if (drop) {
			ndev->stat.tx.drop[rte_lcore_id()][phase]++;
		} else {
			ndev->stat.tx.xmit[rte_lcore_id()][phase]++;
		}
	}	
}

#ifdef TRACE_MBUF
static inline void trace_mbuf(rte_mbuf *mbuf)
{
	
}
#else
static inline void trace_mbuf(rte_mbuf *mbuf) {}
#endif

#define TRACE_DROP_MBUF(m, in, p) do {	\
	stat_mbuf(m, 1, in, p);				\
	trace_mbuf(m);						\
} while(0)

#ifdef __cplusplus
}
#endif

#endif
