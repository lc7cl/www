#ifndef _UTILS_H_
#define _UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common/dns.h>

int print_dns_name(char *out, struct dns_name *in);
int format_domain(struct dns_name *out, char *in, int size, int dot_end);

#ifdef __cplusplus
}
#endif

#endif
