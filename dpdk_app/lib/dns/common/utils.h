#ifndef _UTILS_H_
#define _UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common/dns.h>

struct dns_buf {
	char *last;
	char *cur;
};

static inline uint16_t __attribute__((always_inline))
get_uint16(struct dns_buf *buf)
{
	uint16_t val;

	val = rte_be_to_cpu_16(*(uint16_t*)buf->cur);
	buf->last = buf->cur;
	buf->cur += 2;
	return val;
}

static inline uint32_t __attribute__((always_inline))
get_uint32(struct dns_buf *buf)
{
	uint32_t val;

	val = rte_be_to_cpu_32(*(uint32_t*)buf->cur);
	buf->last = buf->cur;
	buf->cur += 4;
	return val;
}

static inline void __attribute__((always_inline)) 
set_uint16(struct dns_buf *buf, uint16_t val)
{
	*(uint16_t*)buf->cur = rte_cpu_to_be_16(val);	
	buf->last = buf->cur;
	buf->cur += 2;
}

static inline void __attribute__((always_inline))
set_uint32(struct dns_buf *buf, uint32_t val)
{
	*(uint32_t*)buf->cur = rte_cpu_to_be_32(val);
	buf->last = buf->cur;
	buf->cur += 4;
}

static inline void __attribute__((always_inline))
buf_copy(char *dst, struct dns_buf *buf, size_t size)
{
	memcpy(dst, buf->cur, size);
	buf->last = buf->cur;
	buf->cur += size;	
}

#ifdef __cplusplus
}
#endif

#endif
