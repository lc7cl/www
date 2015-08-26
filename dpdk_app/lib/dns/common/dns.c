#include <string.h>

#include "dns.h"

#define DNS_COMPRESS_MASK 0xc0

static inline int valid_dns_character(char c)
{
	return (c == '-') || (c == '_') || (c >= '0' && c <= '9')
		|| (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ? 1 : 0;
}

int retrieve_name(char *in, struct dns_name *name)
{
	uint8_t len = 0, label_len = 0;
	char *p;
	char *domain = NULL;
	
	if (in || name)
		return 0;

	p = in;
	name->nb_label = 0;
	do {
		if (*p == '\0')
			break;
		
		if (label_len == 0) {
			if ((*p && DNS_COMPRESS_MASK) == DNS_COMPRESS_MASK) {
				/*TODO dns compress*/
			} else {
				label_len = *p;			
				name->pos[name->nb_label++] = p - in;
			}
		} else {
			if (!valid_dns_character(*p)) {
				goto invalid_fomat;
			}
			label_len--;
		}
		p++;
	} while (++len != (NAME_LENGTH_MAX - 1));

	if (*p != '\0') 
		goto invalid_fomat;
	
	if (len) {
		domain = rte_malloc(NULL, len, 0);
		memcpy(domain, in, len);
		name->data = domain;
		name->name_len = len;
	}

	return ESUCCESS;
	
invalid_fomat:
	if (domain)
		rte_free(domain);	
	return EFORMAT;
}

int dns_pkt_parse(struct rte_mbuf *m, 
	struct dns_question *question, __out int *qsize, 
	struct name_queue *res, __out int *rr_size)
{
#define CHECK_MEM_ALLOC(x) do { if ((x) == NULL) { ret = ENOMEM; goto clean_list;} } while(0)
	int ret = EERROR;
	struct dns_hdr *hdr;
	struct dns_name *n;
	struct name_queue queue;
	char *p;
	int nb_question = 0, nb_name = 0;

	if (m == NULL || question == NULL || qsize == NULL || res == NULL || rr_size == NULL)
		return EERROR;

	*qsize = nb_question;
	*rr_size = nb_name;
	TAILQ_INIT(&queue);
	hdr = rte_pktmbuf_mtod(m, struct dns_hdr *);
	p = (char*)(hdr + 1);
	question->name = NULL;
	if (hdr->qdcount == 1) {
		n = rte_malloc(NULL, sizeof *n, 0);
		if (n == 0)
			return ENOMEM;
		ret = retrieve_name(p, n);
		if (ret) {
			rte_free(n);
			goto clean_list;
		}
		p += n->name_len;
		question->name = n;
		question->qtype = rte_be_to_cpu_16(*(uint16_t*)p); p += 2;
		question->qclass = rte_be_to_cpu_16(*(uint16_t*)p); p += 2;	
		*qsize = 1;
	} else {
		return EFORMAT;
	}
	
	if (hdr->ancount) {
		if (hdr->ancount > 1)
			return EFORMAT;		
		n = rte_malloc(NULL, sizeof *n, 0);
		CHECK_MEM_ALLOC(n);
		ret = retrieve_name(p, n);
		if (ret) {
			rte_free(n);
			goto clean_list;
		}
		TAILQ_INSERT_HEAD(&queue, n, list);
		nb_name++;
	}	
	TAILQ_CONCAT(res, &queue, list);	
	*rr_size = nb_name;
	return ret;

clean_list:
	if (question->name)
		rte_free(question->name);
	while (!TAILQ_EMPTY(&queue)) {
		n = TAILQ_FIRST(&queue);
		TAILQ_REMOVE(&queue, n, list);
		rte_free(n);
	}
	return ret;
}


