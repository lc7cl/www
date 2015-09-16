#include "client.h"

struct rte_mempool *dns_client_pool;

struct dns_client* dns_client_create(struct rte_mempool *mm_pool, uint32_t addr, uint16_t port)
{
	struct dns_client *client;

	if (rte_mempool_get(mm_pool, &client) < 0)
	 	return NULL;
	client->addr = addr;
	client->port = port;
	rte_rwlock_init(&client->rwlock);
	TAILQ_INIT(&client->query_list);
	rte_atomic32_set(&client->refcnt, 1);
	return client;
}

