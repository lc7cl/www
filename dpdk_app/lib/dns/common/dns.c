#include <string.h>
#include <rte_byteorder.h>

#include "utils.h"
#include "dns.h"

#define DNS_COMPRESS_MASK 0xc0

static inline int valid_dns_character(char c)
{
	return (c == '-') || (c == '_') || (c >= '0' && c <= '9')
		|| (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ? 1 : 0;
}

int retrieve_name(struct dns_buf *buf, struct dns_name *name)
{
	uint8_t len = 0, label_len = 0;
	char *p;
	char *domain = NULL;
	
	if (buf == NULL || name == NULL)
		return EERROR;

	p = buf->cur;
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
				name->pos[name->nb_label++] = p - buf->cur;
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
	memcpy(domain, buf->cur, len);
	name->data = domain;
	name->name_len = len;
	buf->last = buf->cur;
	buf->cur = p;

	return ESUCCESS;
	
invalid_fomat:
	if (domain)
		rte_free(domain);	
	return EFORMAT;
}

int retrieve_question(struct dns_buf *buf, struct dns_question *question)
{
	int ret;
	
	ret = retrieve_name(buf, question->name);
	if (ret != ESUCCESS) 
		return ret;
	question->qtype = get_uint16(buf);
	question->qclass = get_uint16(buf);
	return ESUCCESS;
}

/**
* TODO:check if rr is valid 
*/
int retrieve_rr(struct dns_buf *buf, struct dns_rr *rr, struct rte_mempool *name_pool)
{
	int ret;

	if (rte_mempool_get(name_pool, (void**)&rr->name) < 0)
		return ENOMEMORY;
	ret = retrieve_name(buf, rr->name);
	if (ret != ESUCCESS) {
		goto exit_release_name;
	}

	rr->type = get_uint16(buf);
	rr->class = get_uint16(buf);
	rr->ttl = get_uint32(buf);
	rr->rdlength = get_uint16(buf);
	rr->rdata = rte_malloc("RRDATA", rr->rdlength + 1, 0);
	if (rr->rdata == NULL) {
		ret = ENOMEMORY;
		goto exit_release_name;
	}
	buf_copy(rr->rdata, buf, rr->rdlength);
	rr->rdata[rr->rdlength] = '\0';

	return ESUCCESS;
exit_release_name:
	rte_mempool_put(name_pool, rr->name);
	return ret;
}

int retrieve_rrset(struct dns_buf *buf, struct dns_section *section, int nb, struct rte_mempool *rr_pool, struct rte_mempool *name_pool)
{
	int ret, i;
	struct dns_rr *last, *rr;

	if (nb == 0)
		return ESUCCESS;

	last = TAILQ_LAST(&section->rrset, dns_rr_queue);
	for (i = 0; i < nb; i++) {
		if (rte_mempool_get(rr_pool, (void**)&rr) < 0) {
			ret = ENOMEMORY;
			goto exit_clean_rr;
		}
			
		ret = retrieve_rr(buf, rr, name_pool);
		if (ret != ESUCCESS) {
			goto exit_clean_rr;
		}
		TAILQ_INSERT_TAIL(&section->rrset, rr, list);
	}
	return ESUCCESS;

exit_clean_rr:

	while (i != 0) {
		rr = TAILQ_NEXT(last, list);
		TAILQ_REMOVE(&section->rrset, rr, list);
		rte_mempool_put(rr_pool, rr);
	}
	return ret;
}
