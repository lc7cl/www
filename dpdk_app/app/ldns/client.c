#include "client.h"

struct dns_client* dns_client_alloc(struct dns_mempool *mm_pool, uint32_t addr, uint16_t port)
{
	struct dns_client *client;

	if (rte_mempool_get(mm_pool->client_pool, &client) < 0)
	 	return NULL;
	client->addr = addr;
	client->port = port;
	rte_rwlock_init(&client->rwlock);
	rte_atomic32_set(&client->refcnt, 1);
	TAILQ_INIT(&client->querylist);
	client->mm_pool = mm_pool;
	return client;
}

void dns_client_free(struct dns_client *client)
{
	rte_mempool_put(client->mm_pool->client_pool, client);
}

int dns_client_add_query(struct dns_client *client, struct dns_query *query)
{
	int ret = 0;

	rte_rwlock_write_lock(&client->rwlock);
	rte_atomic32_inc(&client->refcnt);
	query->client = client;
	TAILQ_INSERT_TAIL(&client->querylist, query, list);
	rte_rwlock_write_unlock(&client->rwlock);

	return ret;
}


