#ifndef _REQUEST_H_
#define _REQUEST_H_

#include <recursion/query.h>

struct dns_request_key {
	uint16_t dnsid;	
	uint16_t sport;
	uint32_t dst_addr;
	//uint32_t src_addr;	
	//uint16_t dport;
} __attribute__((__packed__));

struct dns_request {
	struct dns_request_key key;
	TAILQ_ENTRY(dns_request) list;
	struct dns_question question;
	rte_atomic32_t refcnt;
	struct dns_query_queue query;
	rte_rwlock_t rwlock;
};
TAILQ_HEAD(dns_request_queue, dns_request);

int generate_dns_request_key(struct dns_request_key *key);
struct dns_request* dns_request_alloc(struct dns_question *question);
void dns_request_free(struct dns_request *request);
struct dns_request* dns_request_lookup(struct dns_request_key *key);
struct dns_request* dns_request_lookup2(struct dns_name *name, uint16_t qtype, uint16_t qclass);

#endif
