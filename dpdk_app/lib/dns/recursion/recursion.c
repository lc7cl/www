#include "librecursion.h"

int start_recursion(__rte_unused struct dns_query *query)
{
#ifdef CONFIG_DNS_CACHE

#else
	dns_
#endif
    return 0;
}

int resume_recursion(__rte_unused struct dns_query *query)
{	
    return 0;
}


