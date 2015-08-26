#include "utils.h"

int print_dns_name(char *out, int size, struct dns_name *in)
{
	int i, len;

	DNS_ASSERT(out != NULL && in != NULL);

	len = in->name_len - 1;
	if (size - 1 < len)
		return 0;

	memcpy(out, in->data + 1, len);

	for (i = 1; i < in->nb_label; i++) {
		*out[in->pos[i]] = '.';
	}
	*out[len - 1] = '.';
	*out[len] = '\0';
	return len;	
}

int format_domain(struct dns_name *out, char *in, int size, int dot_end)
{
	char *domain, *label_pos, *p;
	int truesize, i;
	
	DNS_ASSERT(out != NULL && in != NULL);

	if (dot_end)
		truesize = size + 1;
	else
		truesize = size + 2;

	domain = rte_malloc(NULL, truesize, 0);
	if (domain == NULL) 
		return ENOMEMORY;

	memcpy(domain + 1, in, truesize - 1);

	out->nb_label = 0;
	label_pos = domain;
	out->pos[out->nb_label++] = label_pos;
	for (i = 0; i < truesize - 1; i++) {
		if (domain[i] == '.') {
			*label_pos = domain + i - label_pos;
			label_pos = domain + i;
			out->pos[out->nb_label++] = label_pos;
		}
	}	
	if (i == truesize - 1 && !dot_end) {
		*label_pos = domain + i - label_pos;
	}
	domain[truesize - 1] = '\0';
	out->data = domain;
	out->name_len = truesize;
	return 0;	
}


