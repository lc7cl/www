#include "request.h"

int generate_dns_request_key(struct dns_request_key *key)
{
	if (key == NULL)
		return -1;

	key
}

struct dns_request* dns_request_alloc(struct dns_question *question)
{
	struct dns_request *new;

	new = rte_malloc(NULL, sizeof(dns_request), 0);
	if (new == NULL)
		return NULL;

	new->key
	
}
