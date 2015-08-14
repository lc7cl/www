#ifndef _SK_H_
#define _SK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <rte_mbuf.h>
#include <rte_ring.h>

struct sk {
	void (*func)(struct rte_mbuf*);
	struct rte_ring backlog;
	struct rte_ring write_ring;
	unsigned lcore_id;
};

#ifdef __clpusplus
}
#endif

#endif
