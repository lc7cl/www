#include <rte_jhash.h>
#include <common/name.h>
#include "query.h"

static struct dns_query_hash *query_hash;

void dns_query_free(struct dns_query* query)
{
	dns_name_put(query->name);
	rte_mempool_put(query->mm_pool->query_pool, query);
}

struct dns_query* dns_query_alloc(struct dns_mempool *pool, 
	struct dns_name *name, uint16_t type, uint16_t class)
{
	struct dns_query *query, *new, *retval;
	struct dns_query_slot* slot;

	if (name == NULL) 
		return NULL;

	if (rte_mempool_get(pool->query_pool, (void**)&new) < 0) {
		return NULL;
	}

	dns_name_get(name);
	new->name = name;
	new->type = type;
	new->class = class;
	new->hval = dns_query_get_hash(query_hash, name, type, class);
	new->client_addr = 0;
	new->rcode = 0;
	new->mm_pool = pool;
	new->state = DNS_QUERY_STATE_NONE;

	/*check if query with <name, type, class> exits?if exists, free new and return the existing one*/
	slot = dns_query_get_slot(query_hash, dns_query_get_hash(query_hash, name, type, class));
	rte_rwlock_write_lock(&slot->rwlock);
	TAILQ_FOREACH(query, &slot->head, list) {
		if (query->state != DNS_QUERY_STATE_DELETED
			&& dns_name_equal(query->name, name)
			&& query->class == class
			&& query->type == type) {
			rte_atomic32_inc(&query->refcnt);
			retval = query;
			dns_query_free(new);
			goto unlock_return;
		}
	}
	
	rte_atomic32_inc(&new->refcnt);
	retval = new;	
unlock_return:
	rte_rwlock_write_unlock(&slot->rwlock);
	return retval;	
}

struct dns_query* dns_query_lookup(struct dns_query_hash *hash, 
	struct dns_name *name, uint16_t type, uint16_t class, int noref)
{
	struct dns_query *query;
	struct dns_query_slot* slot;

	slot = dns_query_get_slot(hash, dns_query_get_hash(hash, name, type, class));
	rte_rwlock_read_lock(&slot->rwlock);
	TAILQ_FOREACH(query, &slot->head, list) {
		if (query->state != DNS_QUERY_STATE_DELETED
			&& dns_name_equal(query->name, name)
			&& query->class == class
			&& query->type == type) {
			if (!noref)
				rte_atomic32_inc(&query->refcnt);
			rte_rwlock_read_unlock(&slot->rwlock);	
			return query;				
		}
	}
	rte_rwlock_read_unlock(&slot->rwlock);	
	return NULL;
}

int dns_query_hash_init(int shift) 
{
	int i, alloc_size;

	alloc_size = sizeof(struct dns_query_hash) + (1 << shift) * sizeof(struct dns_query_slot);
	query_hash = rte_zmalloc(NULL, alloc_size, 0);
	if (query_hash == NULL)
		return -1;

	query_hash->hash = rte_jhash;
	query_hash->slot_shift = shift;
	query_hash->slot_mask = ~(1 << shift);

	for (i = 0; i < 1 << shift; i++) {
		rte_rwlock_init(&query_hash->slot[i].rwlock);
		query_hash->slot[i].nb = 0;
	}

	return 0;	
}
