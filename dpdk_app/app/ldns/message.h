#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <common/dns.h>
#include "client.h"

struct dns_message {
	uint32_t client_ip;
	struct dns_question question;
	struct dns_name_queue section[SECTION_MAX];
	TAILQ_ENTRY(dns_message) list;
	struct dns_client *client;
};
TAILQ_HEAD(dns_message_queue, dns_message);

#endif