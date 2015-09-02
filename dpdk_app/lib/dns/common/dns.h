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

enum DNS_SECTION {
	SECTION_QUESTION,
	SECTION_ANSWER,
	SECTION_AUTHORITY,
	SECTION_ADDITIONAL,
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

struct dns_rr;
TAILQ_HEAD(rrlist, dns_rr);

struct dns_name {
	TAILQ_ENTRY(dns_name) list;
	char *data;
	uint8_t pos[NAME_LENGTH_MAX];
	uint8_t nb_label;
	uint8_t name_len;
	struct rrlist head;
};
TAILQ_HEAD(dns_name_queue, dns_name);

struct dns_question {
	struct dns_name name;
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
	char rdata[0];
};
TAILQ_HEAD(dns_rr_queue, dns_rr);

int retrieve_name(char *in, struct dns_name *name);
int dns_pkt_parse(struct rte_mbuf *m, 
	struct dns_question *question, __out int *qsize, 
	struct dns_name_queue *res, __out int *size);

#ifdef __cplusplus
}
#endif

#endif
