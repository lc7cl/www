#ifndef _QUERY_H_
#define _QUERY_H_

#include <common/dns.h>

enum dns_query_state {
	DNS_QUERY_STATE_NONE,
	DNS_QUERY_STATE_PARSING,	
	DNS_QUERY_STATE_COMPLETE,
	DNS_QUERY_STATE_DELETED,
};

struct dns_query {
	uint32_t hval;
	struct dns_name *name;
	uint16_t type;
	uint16_t class;
	uint32_t client_addr;
	struct dns_message *request;
	struct dns_message *response;
	struct dns_client *client;
	int rcode;
	uint32_t state;
	rte_atomic32_t refcnt;
	struct dns_mempool *mm_pool;
	TAILQ_ENTRY(dns_query) list;
};
TAILQ_HEAD(dns_query_queue, dns_query);

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

struct dns_query* dns_query_alloc(struct rte_mempool *pool, 
	struct dns_name *name, uint16_t type, uint16_t class);
struct dns_query* dns_query_free(struct dns_query* query);
struct dns_query* dns_query_lookup(struct dns_name *name, uint16_t type, uint16_t class, int create);
int dns_query_hash_init(int shift);

static inline uint32_t __attribute__((always_inline)) 
dns_query_get_hash(struct dns_query_hash *hash, 
	struct dns_name *name, uint16_t type, uint16_t class)
{
	uint32_t hval;
	uint32_t val;

	val = type << 16 + class & 0xFFFF;
	hval = hash->hash(name->data, name->name_len, val);
	return hval;
}

static inline struct dns_query_slot* __attribute__((always_inline))
dns_query_get_slot(struct dns_query_hash *hash, uint32_t hval)
{
	return &hash->slot[hval & hash->slot_mask];
}

static inline void __attribute__((always_inline))
dns_query_get(struct dns_query *query)
{
	rte_atomic32_inc(&query->refcnt);
}

static inline void __attribute__((always_inline))
dns_query_put(struct dns_query *query)
{
	if(rte_atomic32_dec_and_test(&query->refcnt)) {
		dns_query_free(query);
	}
}

#endif
