#ifndef _SRVADDR_H_
#define _SRVADDR_H_

#include <sys/queue.h>
#include <rte_rwlock.h>

struct srvaddr {
	TAILQ_ENTRY(srvaddr) list;
	uint32_t addr;
	int delete;
	int permanent;
	int rtt;
	uint64_t update_time;
	rte_atomic32_t refcnt;
	rte_rwlock_t rwlock;
};
TAILQ_HEAD(srvaddr_queue, srvaddr);

struct srvaddr* srvaddr_alloc(uint32_t addr, int permanent);

void srvaddr_free(struct srvaddr* srv);

#endif
