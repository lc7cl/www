#include <rte_malloc.h>
#include <rte_cycles.h>

#include "srvaddr.h"

struct srvaddr* srvaddr_alloc(uint32_t addr, int permanent)
{
	struct srvaddr* srv;
	
	srv = rte_malloc(NULL, sizeof(struct srvaddr), 0);
	if (srv == NULL)
		return NULL;

	srv->addr = addr;
	srv->delete = 0;
	srv->permanent = permanent;
	srv->rtt = 0;
	srv->update_time = rte_get_tsc_cycles();
	rte_atomic32_set(&srv->refcnt, 1);
	rte_rwlock_init(&srv->rwlock);
	return srv;
}

void srvaddr_free(struct srvaddr* srv)
{
	if (rte_atomic32_dec_and_test(&srv->refcnt)) {
		rte_free(srv);
	}
}


