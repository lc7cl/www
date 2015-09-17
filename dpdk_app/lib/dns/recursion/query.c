#include "query.h"

struct dns_query_slot {
	int nb;
	struct rte_rwlock_t rwlock;	
	struct dns_query_queue head;
};

struct dns_query_hash {
	uint32_t (*hash)(const void*, uint32_t, uint32_t);
	int slot_shift;
	int slot_mask;
	struct dns_query_slot *slot;
};

static struct dns_query_hash *query_hash;

uint32_t dns_query_get_hash(struct dns_query_hash *hash, 
	struct dns_name *name, uint16_t type, uint16_t class)
{
	uint32_t hval;
	uint32_t val;

	val = type << 16 + class & 0xFFFF;
	hval = hash->hash(name->data, name->name_len, val);
	return hval;
}

static inline struct dns_query_slot* 
dns_query_get_slot(struct dns_query_hash *hash, uint32_t hval)
{
	return &hash->slot[hval & hash->slot_mask];
}

struct dns_query* dns_query_lookup(struct dns_query_hash *hash, 
	struct dns_name *name, uint16_t type, uint16_t class, int create)
{
	struct dns_query *query;
	struct dns_query_slot* slot;

	slot = dns_query_get_slot(hash, dns_query_get_hash(hash, name, type, class));

	rte_rwlock_read_lock(&slot->rwlock);
	TAILQ_FOREACH(query, &slot->head, list) {
		if (query->state != DNS_QUERY_STATE_DELETED
			&& )

	}
	rte_rwlock_read_unlock(&slot->rwlock);	
}


int dns_query_hash_init(int shift) 
{
	int alloc_size, i;

	alloc_size = sizeof(struct dns_query_hash) + (1 << shift) * sizeof(struct dns_query_bucket);
	query_hash = rte_zmalloc(NULL, sizeof(struct dns_query_hash), 0);
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


