#ifndef _NAME_H_
#define _NAME_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common/dns.h>

int dns_name_equal(struct dns_name *name1, struct dns_name *name2);
int dns_name_is_zone(struct dns_name *maybe_zone, struct dns_name *maybe_subzone);
int dns_name_dump(char *out, int size, struct dns_name *in);
int dns_name_format(struct dns_name *out, char *in, int size, int dot_end);
void dns_name_free(struct dns_name *name);

static inline void __attribute__((always_inline)) 
dns_name_get(struct dns_name *name)
{
	rte_atomic32_inc(&name->refcnt);
}

static inline void __attribute__((always_inline)) 
dns_name_put(struct dns_name *name)
{
	if (rte_atomic32_dec_and_test(&name->refcnt)) {
		dns_name_free(name);
	}
}

#ifdef __cplusplus
}
#endif

#endif
