#include "message.h"

int message_retrieve(struct rte_mbuf *mbuf, struct dns_message *msg, struct dns_mempool *mm_pool)
{
	struct dns_hdr *dnshdr;
	struct dns_question *question;
	struct dns_buf buf;
	int ret;

	question = NULL;
	
	dnshdr = rte_pktmbuf_mtod(mbuf, struct dns_hdr *);
	msg->rcode = dnshdr->rcode;
	msg->request = dnshdr->qr;
	
	buf.last = (char *)(dnshdr + 1);
    buf.cur = (char *)(dnshdr + 1);

	/*proccess question*/
	if (dnshdr->qdcount == 1) {
		if (rte_mempool_get(mm_pool->question_pool, (void**)&question) < 0) {
			return ENOMEMORY;
		}
		ret = retrieve_question(&buf, question);
		if (ret != ESUCCESS) {
			rte_mempool_put(mm_pool->question_pool, question);
			return ret;
		}		
	} else if (dnshdr->qdcount > 1) {
		return EFORMAT;
	}
	msg->question = question;

	/*process answer*/
	ret = retrieve_rrset(&buf, &msg->section[SECTION_ANSWER], dnshdr->ancount, mm_pool->rr_pool, mm_pool->name_pool);
	if (ret != ESUCCESS)
		return ret;

	/*process authorization*/	
	ret = retrieve_rrset(&buf, &msg->section[SECTION_AUTHORITY], dnshdr->nscount, mm_pool->rr_pool, mm_pool->name_pool);
	if (ret != ESUCCESS)
		return ret;

	/*process additional*/	
	ret = retrieve_rrset(&buf, &msg->section[SECTION_ADDITIONAL], dnshdr->arcount, mm_pool->rr_pool, mm_pool->name_pool);
	if (ret != ESUCCESS)
		return ret;
	
	return 0;
}
