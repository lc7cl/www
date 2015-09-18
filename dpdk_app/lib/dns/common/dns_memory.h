#ifndef _DNS_MEMORY_H_
#define _DNS_MEMORY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common/dns.h>

struct dns_mempool {
	struct rte_mempool *name_pool;
	struct rte_mempool *question_pool;
	struct rte_mempool *rr_pool;
	struct rte_mempool *query_pool;
	struct rte_mempool *message_pool;
	struct rte_mempool *client_pool;
};

#ifdef __cplusplus
}
#endif

#endif
