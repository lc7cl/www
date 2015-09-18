#ifndef _DNS_H_
#define _DNS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/queue.h>
#include <rte_common.h>
#include <rte_malloc.h>
#include <rte_mbuf.h>

typedef uint32_t be32;

#define __in
#define __out

#define LABEL_LENGTH_MAX (64)
#define NAME_LENGTH_MAX (256)

#define DNS_ASSERT(x) if ( !(x) ) rte_panic("dns panic!!!\n")

enum {
	ESUCCESS = 0,
	EERROR,
	ENOMEMORY,
	EFORMAT,
	ENORR,

	EMAX
};

enum DNS_QTYPE {
	DNS_QTYPE_A 	= 1,
	DNS_QTYPE_NS 	= 2,
	DNS_QTYPE_MD	= 3,
	DNS_QTYPE_MF 	= 4,
	DNS_QTYPE_CNAME = 5,
	DNS_QTYPE_SOA 	= 6,
	DNS_QTYPE_MB 	= 7,
	DNS_QTYPE_MG 	= 8,
	DNS_QTYPE_MR 	= 9,
	DNS_QTYPE_NULL 	= 10,
	DNS_QTYPE_WKS 	= 11,
	DNS_QTYPE_PTR 	= 12,
	DNS_QTYPE_HINFO = 13,
	DNS_QTYPE_MINFO = 14,
	DNS_QTYPE_MX 	= 15,
	DNS_QTYPE_TXT 	= 16,
	DNS_QTYPE_AXFR 	= 252,
	DNS_QTYPE_MAILB	= 253,
	DNS_QTYPE_MAILA	= 254,
	DNS_QTYPE_ALL	= 255
};

enum DNS_CLASS {
	DNS_CLASS_IN 	= 1,
	DNS_CLASS_CS 	= 2,
	DNS_CLASS_CH 	= 3,
	DNS_CLASS_HS 	= 4,
	DNS_CLASS_ALL 	= 255
};

enum DNS_SECTION {
	SECTION_QUESTION = 0,
	SECTION_ANSWER,
	SECTION_AUTHORITY,
	SECTION_ADDITIONAL,
	SECTION_MAX,
};

struct dns_hdr {
	uint16_t id;
	uint8_t rd:1,
			tc:1,
			aa:1,
			opcode:4,
			qr:1;
	uint8_t rcode:4,
			z:3,
			ra:1;
	uint16_t qdcount;
	uint16_t ancount;
	uint16_t nscount;
	uint16_t arcount;			
} __attribute((__packed__))__;

struct dns_buf {
	char *last;
	char *cur;
};

struct dns_name {
	TAILQ_ENTRY(dns_name) list;
	char *data;
	uint8_t pos[NAME_LENGTH_MAX];
	uint8_t nb_label;
	uint8_t name_len;
	rte_atomic32_t refcnt;
	struct rte_mempool *name_pool;
	//struct dns_rr_queue head;
};
TAILQ_HEAD(dns_name_queue, dns_name);

struct dns_question {
	struct dns_name *name;
	uint16_t qtype;
	uint16_t qclass;
    TAILQ_ENTRY(dns_question) list;
};
TAILQ_HEAD(dns_question_queue, dns_question);

struct dns_rr {	
	TAILQ_ENTRY(dns_rr) list;
	struct dns_name *name;
	uint16_t type;
	uint16_t class;
	uint32_t ttl;
	uint16_t rdlength;
	char *rdata;
};
TAILQ_HEAD(dns_rr_queue, dns_rr);

struct dns_section {
	struct dns_rr_queue rrset;
	int nb_rr;
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

int retrieve_name(struct dns_buf *buf, struct dns_name *name);
int retrieve_question(struct dns_buf *buf, struct dns_question *question);
int retrieve_rr(struct dns_buf *buf, struct dns_rr *rr, struct rte_mempool *name_pool);
int retrieve_rrset(struct dns_buf *buf, struct dns_section *section, int nb, 
	struct rte_mempool *rr_pool, struct rte_mempool *name_pool);

#ifdef __cplusplus
}
#endif

#endif
