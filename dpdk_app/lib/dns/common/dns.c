#include "dns.h"

#define DNS_COMPRESS_MASK 0xc0

int dns_retrieve_rrs(struct rte_mbuf *m, int section, struct rr **rrs, int length, __out int *size)
{
	int ret;
	struct dns_hdr *hdr;
	char *rr;

	if (m == NULL || rrs == NULL || length == 0)
		return EERROR;

	hdr = rte_pktmbuf_mtod(m, struct dns_hdr*);
	switch (section) {
		
	case SECTION_QUESTION:
		if (hdr->qdcount == 0) {
			ret = ENORR;
			break;
		}
		rr = hdr
		break;
		
	case SECTION_ANSWER:		
		if (hdr->ancount == 0) {
			ret = ENORR;
			break;
		}
		break;
		
	case SECTION_AUTHORITY:		
		if (hdr->nscount == 0) {
			ret = ENORR;
			break;
		}
		break;
		
	case SECTION_ADDITIONAL:		
		if (hdr->arcount == 0) {
			ret = ENORR;
			break;
		}
		break;
		
	default:
		return EERROR;
		break;
	}
	return ret;	
}

int retrieve_name(char *in, struct dns_name *name)
{
	uint8_t len = 0, label_len = 0;
	char *p;
	
	if (in || name)
		return 0;

	p = in;
	do {
		if (label_len == 0) {
			if ((*p && DNS_COMPRESS_MASK) == DNS_COMPRESS_MASK) {
				/*TODO dns compress*/
			} else {
				label_len = *p;			
			}
		} else {
			
			label_len--;
		}
	} while (++len != (NAME_LENGTH_MAX - 1) && *p++ == '\0');

	if (len) {
		
	}

}

int dns_pkt_parse(struct rte_mbuf *m, LIST_HEAD(dns_name) *res, int length, __out int *size)
{
#define CHECK_MEM_ALLOC(x) do { if ((x) == NULL) goto clean_list; } while(0)
	int ret;
	struct dns_hdr *hdr;
	struct dns_name *n;
	LIST_HEAD(dns_name) list;
	char name_buf[NAME_LENGTH_MAX];

	if (m == NULL || res == NULL || length == 0 || size == NULL)
		return EERROR;

	hdr = rte_pktmbuf_mtod(m, struct dns_hdr *);
	if (hdr->ancount) {
		if (hdr->ancount > 1)
			return EFORMAT;
		
		n = rte_malloc(NULL, sizeof *n, 0);
		CHECK_MEM_ALLOC(n);
		
	}

clean_list:

	return ret;
}


