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
	struct dns_name *name;
	uint16_t type;
	uint16_t class;
	uint32_t client_addr;
	int rcode;
	uint32_t state;
	TAILQ_ENTRY(dns_query) list;
};
TAILQ_HEAD(dns_query_queue, dns_query);

struct dns_query* dns_query_alloc();
struct dns_query* dns_query_free();
struct dns_query* dns_query_lookup(struct dns_name *name, uint16_t type, uint16_t class, int create);
int dns_query_hash_init(int shift) ;

#endif