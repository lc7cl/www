#ifndef _QUERY_H_
#define _QUERY_H_

struct dns_query {
	struct dns_client *client;
	struct dns_question question;
	struct dns_name_queue stack;
	struct dns_message in;
	struct dns_message res;
	int rcode;
	TAILQ_ENTRY(dns_query) list;
};
TAILQ_QUEUE(dns_query_queue, dns_query);

#endif