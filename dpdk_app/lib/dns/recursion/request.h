#ifndef _REQUEST_H_
#define _REQUEST_H_

#include <query.h>

struct dns_request {
	TAILQ_ENTRY(dns_request) list;
	rte_atomic32_t refcnt;
	struct dns_query_queue query;
};
TAILQ_HEAD(dns_request_queue, dns_request);

#endif