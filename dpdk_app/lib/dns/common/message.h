#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <common/dns_memory.h>

struct dns_message {
	struct dns_question *question;
	struct dns_section section[SECTION_MAX];
	TAILQ_ENTRY(dns_message) list;
	//struct dns_client *client;
	struct dns_mempool *pool;
	int rcode;
	int request;
};
TAILQ_HEAD(dns_message_queue, dns_message);

int message_retrieve(struct rte_mbuf *mbuf, struct dns_message *msg, struct dns_mempool *mm_pool);

#endif
