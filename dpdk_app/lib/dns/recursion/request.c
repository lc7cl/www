#include "request.h"

int generate_dns_request_key(struct dns_request_key *key)
{
#define KEY_GENERATE_TRIES 32
	uint32_t rand;
	int tries = KEY_GENERATE_TRIES;
	uint16_t dnsid;
	uint16_t sport;
	
	if (key == NULL)
		return -1;

	rand = lrand48();
	while (tries > 0) {		
		dnsid = rand >> 16;
		sport = rand & 0xff;
		sport = (sport < 1024) ? (sport + 1024) : sport;
		key->dnsid = dnsid;
		key->sport = sport;
		if (rte_hash_lookup(request_hash, key)) {
			rand += 
		}
	}	
}

struct dns_request* dns_request_alloc(struct dns_question *question)
{
	struct dns_request *new;

	new = rte_malloc(NULL, sizeof(dns_request), 0);
	if (new == NULL)
		return NULL;

	new->key
	
}

struct dns_request* dns_request_lookup2(struct dns_name *name, uint16_t qtype, uint16_t qclass)
{
	struct dns_request *retval = NULL;

	
}

struct rte_hash* dns_request_hash_create(struct rte_hash_parameters *params)
{
	struct rte_hash *hash;

	hash = rte_hash_create(params);
}

