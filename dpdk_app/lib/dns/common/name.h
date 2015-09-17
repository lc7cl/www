#ifndef _NAME_H_
#define _NAME_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common/dns.h>

int dns_name_equal(struct dns_name *name1, struct *name2);
int dns_name_is_zone(struct dns_name *maybe_zone, struct *maybe_subzone);
int dns_name_dump(char *out, int size, struct dns_name *in);
int dns_name_format(struct dns_name *out, char *in, int size, int dot_end);

#ifdef __cplusplus
}
#endif

#endif
