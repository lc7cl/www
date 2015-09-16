#ifndef _UTILS_H_
#define _UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common/dns.h>


int print_dns_name(char *out, int size, struct dns_name *in);
int format_domain(struct dns_name *out, char *in, int size, int dot_end);

static inline uint16_t get_uint16(char *buf, char **cur)
{
	uint16_t val;

	val = rte_be_to_cpu_16(*(uint16_t*)buf);
	if (cur)
		*cur = buf + 2;
	return val;
}

static inline uint32_t get_uint32(char *buf, char **cur)
{
	uint32_t val;

	val = rte_be_to_cpu_32(*(uint32_t*)buf);
	if (cur)
		*cur = buf + 4;
	return val;
}

static inline void set_uint16(char *buf, uint16_t val, char **cur)
{
	*(uint16_t*)buf = rte_cpu_to_be_16(val);
	if (cur)
		*cur = buf + 2;
}

static inline void set_uint32(char *buf, uint32_t val, char **cur)
{
	*(uint32_t*)buf = rte_cpu_to_be_32(val);
	if (cur)
		*cur = buf + 4;
}


#ifdef __cplusplus
}
#endif

#endif
