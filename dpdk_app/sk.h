#ifndef _SK_H_
#define _SK_H_

struct sk {
	void (*func)(struct rte_mbuf*);
	struct rte_ring backlog;
	struct rte_ring write_ring;
	unsigned lcore_id;
};

#endif
