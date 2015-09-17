#include "message.h"

int message_retrieve(struct rte_mbuf *mbuf, struct dns_message *msg)
{
	struct dns_hdr *dnshdr;
	struct dns_question *question;
	struct dns_buf *buf;
	int ret, i;

	question = NULL;
	
	dnshdr = rte_pktmbuf_mtod(mbuf, struct dns_hdr *);
	msg->rcode = dnshdr->rcode;
	msg->response = dnshdr->qr;
	
	buf->last = buf->cur = (char *)(dnshdr + 1);

	/*proccess question*/
	if (dnshdr->qdcount == 1) {
		if (rte_mempool_get(msg->question_pool, &question) < 0) {
			return ENOMEMORY;
		}
		ret = retrieve_question(buf, question);
		if (ret != ESUCCESS) {
			rte_mempool_put(msg->question_pool, question);
			return ret;
		}		
	} else if (dnshdr->qdcount > 1) {
		return EFORMAT;
	}
	msg->question = question;

	/*process answer*/
	ret = retrieve_rrset(&msg->section[SECTION_ANSWER], buf, msg->rr_pool, msg->name_pool);
	if (ret != ESUCCESS)
		return ret;

	/*process authorization*/	
	ret = retrieve_rrset(&msg->section[SECTION_AUTHORITY], buf, msg->rr_pool, msg->name_pool);
	if (ret != ESUCCESS)
		return ret;

	/*process additional*/	
	ret = retrieve_rrset(&msg->section[SECTION_ADDITIONAL], buf, msg->rr_pool, msg->name_pool);
	if (ret != ESUCCESS)
		return ret;
	
	return 0;
}
