#include <string.h>
#include <rte_byteorder.h>
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
	
	if (in == NULL || name == NULL)
		return EERROR;

	p = in;
	name->nb_label = 0;
	while (len != (NAME_LENGTH_MAX - 1)) {
		len++;
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
	} 

	if (*p != '\0') 
		goto invalid_fomat;
	
	domain = rte_malloc(NULL, len, 0);
	if (domain == NULL) {
		name->data = NULL;
		name->name_len = 0;
		return ENOMEMORY;
	}
	memcpy(domain, in, len);
	name->data = domain;
	name->name_len = len;

	return ESUCCESS;
	
invalid_fomat:
	if (domain)
		rte_free(domain);	
	return EFORMAT;
}

int dns_pkt_parse(struct rte_mbuf *m, 
	struct dns_question *question, __out int *qsize, 
	struct dns_name_queue *res, __out int *rr_size)
{
#define CHECK_MEM_ALLOC(x) do { if ((x) == NULL) { ret = ENOMEM; goto clean_list;} } while(0)

	int ret = EERROR;
	struct dns_hdr *hdr;
	struct dns_name *n;
	struct dns_name_queue queue;
	char *p;
	int nb_question = 0, nb_name = 0;
	int ignore_answer = 0, ignore_authority = 0, ignore_additional = 0;

	if (m == NULL || question == NULL || qsize == NULL || res == NULL || rr_size == NULL)
		return EERROR;

	*qsize = nb_question;
	*rr_size = nb_name;
	TAILQ_INIT(&queue);
	hdr = rte_pktmbuf_mtod(m, struct dns_hdr *);
	p = (char*)(hdr + 1);
	if (rte_be_to_cpu_16(hdr->qdcount) == 1) {
		ret = retrieve_name(p, &question->name);
		if (ret) 
			goto clean_list;

		p += question->name.name_len;
		question->qtype = rte_be_to_cpu_16(*(uint16_t*)p); p += 2;
		question->qclass = rte_be_to_cpu_16(*(uint16_t*)p); p += 2;	
		*qsize = 1;
	} else {
		return EFORMAT;
	}

	/*for dns request, ignore answer/authority section*/
	if (hdr->qr == 0) {
		ignore_answer = 1;
		ignore_authority = 1;
	}
	
	if (!ignore_answer && rte_be_to_cpu_16(hdr->ancount)) {
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

	if (!ignore_authority && rte_be_to_cpu_16(hdr->ancount)) {

	}

	if (!ignore_additional && rte_be_to_cpu_16(hdr->arcount)) {

	}
	
	TAILQ_CONCAT(res, &queue, list);	
	*rr_size = nb_name;
	return ret;

clean_list:
	while (!TAILQ_EMPTY(&queue)) {
		n = TAILQ_FIRST(&queue);
		TAILQ_REMOVE(&queue, n, list);
		rte_free(n);
	}
	return ret;
}


