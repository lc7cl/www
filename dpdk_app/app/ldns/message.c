#include "message.h"

int message_retrieve(struct rte_mbuf *mbuf, struct dns_message *msg)
{
	struct dns_hdr *dnshdr;
	struct dns_name *name;
	struct dns_question *question;
	char *data;
	int ret;

	question = NULL;
	name = NULL;
	
	dnshdr = rte_pktmbuf_mtod(mbuf, struct dns_hdr *);
	data = (char *)(dnshdr + 1);

	/*proccess question*/
	if (dnshdr->qdcount == 1) {
		if (rte_mempool_get(msg->question_pool, &question) < 0) {
			return ENOMEMORY;
		}
		ret = retrieve_question(data, question, &data);
		if (ret != ESUCCESS) {
			rte_mempool_put(msg->question_pool, question);
			return ret;
		}		
	} else if (dnshdr->qdcount > 1) {
		return EFORMAT;
	}

	/*process answer*/
	
	return 0;
}
