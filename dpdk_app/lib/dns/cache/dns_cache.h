#ifndef _DNS_CACHE_H_
#define _DNS_CACHE_H_

#include <common/dns.h>

struct dns_cache_operation {
	int (*lookup)(struct dns_name *name, struct dns_rr_queue *rrset);
};

struct dns_cache {
	struct dns_cache_operation *ops;	
};

#endif
