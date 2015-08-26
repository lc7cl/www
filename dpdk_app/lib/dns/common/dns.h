#ifndef _DNS_H_
#define _DNS_H_

#include <rte_common.h>

#define LABEL_LENGTH_MAX (64)
#define NAME_LENGTH_MAX (256)

enum {
	ESUCCESS = 0,
	EERROR,
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

struct dns_name;

struct rr {	
	TAILQ_ENTRY(rr) list;
	struct dns_name *name;
	uint16_t type;
	uint16_t class;
	uint32_t ttl;
	uint16_t rdlength;
	char rdata[0];
};

TAILQ_HEAD(rrlist,rr);

struct dns_name {
	TAILQ_ENTRY(dns_name) list;
	char *data;
	uint8_t pos[NAME_LENGTH_MAX];
	uint8_t nb_label;
	struct rrlist head;
};

//typedef LIST_HEAD(dns_name) dns_name_list_t;
 
int dns_retrieve_rrs(struct rte_mbuf *m, int section, struct rr **rrs, int length, __out int *size);
int dns_pkt_parse(struct rte_mbuf *m, TAILQ_HEAD(dns_name) *res, __out int *size);
int retrieve_name(char *in, struct dns_name *name);

#endif
