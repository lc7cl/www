#ifndef _DNS_CLIENT_H_
#define _DNS_CLIENT_H_

#include <rte_rwlock.h>
#include <common/dns_memory.h>
#include <recursion/query.h>

struct dns_message_queue;
struct dns_client {
	union {
		uint64_t key_0;
		struct {
    		uint32_t addr;
    		uint16_t port;
		} key_1;
	} key;
    rte_rwlock_t rwlock;	
    rte_atomic32_t refcnt;
	struct dns_query_queue querylist;
	struct dns_mempool *mm_pool;
};

struct dns_client* dns_client_alloc(struct dns_mempool *mm_pool, uint32_t addr, uint16_t port);
void dns_client_free(struct dns_client *client);
struct dns_client* dns_client_lookup(uint32_t addr, uint16_t port, int noref);

int dns_client_add_query(struct dns_client *client, struct dns_query *query);
int dns_client_delete_query(struct dns_client *client, struct dns_query *query);

static inline void __attribute__((always_inline))
dns_client_get(struct dns_client *client)
{
	rte_atomic32_inc(&client->refcnt);
}

static inline void __attribute__((always_inline))
dns_client_put(struct dns_client *client)
{
	if (rte_atomic32_dec_and_test(&client->refcnt)) {
		dns_client_free(client);
	}
}

#endif
