#include <rte_hash.h>
#include "client.h"

static struct rte_hash *dns_client_hash;
static rte_rwlock_t hash_rwlock = RTE_RWLOCK_INITIALIZER;

struct dns_client* dns_client_alloc(struct dns_mempool *mm_pool, uint32_t addr, uint16_t port)
{
	struct dns_client *client, *new, *retval;

	if (rte_mempool_get(mm_pool->client_pool, (void**)&new) < 0)
	 	return NULL;
	new->key.key_0 = ((uint64_t)port << 32) + (uint64_t)addr;
	rte_rwlock_init(&new->rwlock);
	TAILQ_INIT(&client->querylist);
	new->mm_pool = mm_pool;
	rte_rwlock_write_lock(&hash_rwlock);
	if (rte_hash_lookup_data(dns_client_hash, &new->key, (void**)&client) < 0) {
		rte_hash_add_key_data(dns_client_hash, &new->key, new);
		rte_atomic32_set(&new->refcnt, 1);
		retval = new;
	} else {
		rte_mempool_put(mm_pool->client_pool, new);
		rte_atomic32_inc(&client->refcnt);
		retval = client;
	}		
	rte_rwlock_write_unlock(&hash_rwlock);
	return retval;
}

void dns_client_free(struct dns_client *client)
{
	rte_rwlock_write_lock(&hash_rwlock);
	rte_hash_del_key(dns_client_hash, &client->key);	
	rte_rwlock_write_unlock(&hash_rwlock);
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
	query->mm_pool = client->mm_pool;

	return ret;
}

int dns_client_delete_query(struct dns_client *client, struct dns_query *query)
{
	rte_rwlock_write_lock(&client->rwlock);
	TAILQ_REMOVE(&client->querylist, query, list);
	rte_rwlock_write_unlock(&client->rwlock);
	return 0;
}

struct dns_client* dns_client_lookup(uint32_t addr, uint16_t port, int noref)
{
	struct dns_client *client;
	uint64_t key;

	key = ((uint64_t)port << 32) + (uint64_t)addr;

	rte_rwlock_read_lock(&hash_rwlock);
	if (rte_hash_lookup_data(dns_client_hash, &key, (void**)&client) < 0) {
		rte_rwlock_read_unlock(&hash_rwlock);
		return NULL;
	}
	rte_rwlock_read_unlock(&hash_rwlock);

	if (!noref)
		rte_atomic32_inc(&client->refcnt);
	

	return client;
}
