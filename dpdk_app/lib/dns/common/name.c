#include <string.h>
#include "name.h"

int dns_name_equal(struct dns_name *name1, struct *name2)
{
	if (name1->name_len != name2->name_len
		|| name1->nb_label != name2->nb_label)
		return 0;
	
	if (memcmp(name1->data, name2->data, name1))
		return 0;

	return 1;
}

int dns_name_is_zone(struct dns_name *maybe_zone, struct *maybe_subzone)
{
	if (maybe_zone->name_len > maybe_subzone->name_len
		|| maybe_zone->nb_label > maybe_subzone->nb_label)
		return 0;

	if (memcmp(maybe_zone->data, maybe_subzone->data, maybe_zone->pos[maybe_zone->nb_label - 1]))
		return 0;

	return 1;
}

int dns_name_dump(char *out, int size, struct dns_name *in)
{
	int i, len;

	DNS_ASSERT(out != NULL && in != NULL);

	len = in->name_len - 1;
	if (size - 1 < len)
		return 0;

	memcpy(out, in->data + 1, len);

	for (i = 1; i < in->nb_label; i++) {
		out[in->pos[i] - 1] = '.';
	}
	out[len - 1] = '.';
	out[len] = '\0';
	return len;	
}

int dns_name_format(struct dns_name *out, char *in, int size, int dot_end)
{
	char *domain, *label_pos;
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
	out->pos[out->nb_label++] = label_pos - in;
	for (i = 0; i < truesize - 1; i++) {
		if (domain[i] == '.') {
			*label_pos = domain + i - label_pos;
			label_pos = domain + i;
			out->pos[out->nb_label++] = label_pos - in;
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

void dns_name_free(struct dns_name *name)
{
	rte_mempool_put(name->name_pool, name);
}

