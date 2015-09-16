#include <string.h>
#include <rte_malloc.h>

#include "port_cfg.h"

int port_add_request_domain(struct port_conf *cfg, char *domain, int type)
{
	struct request_data *request;

	request = cfg->req_head;
	while (request) {
		if (!strcmp(request->domain, domain)
			&& request->type == type) {
			return 0;
		}
		request = request->next;
	}
	request = rte_malloc(NULL, sizeof(struct request_data) + strlen(domain) + 1, 0);
	if (request == NULL)
		return -1;
	strcpy(request->domain, domain);
	request->type = type;
	request->next = NULL;
	if (cfg->req_head == NULL) {
		cfg->req_head = request;
		cfg->req_tail = request;
	} else {
		cfg->req_tail->next = request;
		cfg->req_tail = request;
	}
	return 0;		
}

int port_add_request_file(struct port_conf *cfg, char *file)
{
	if (cfg->request_file)
		return -1;
	cfg->request_file = strdup(file);
	if (cfg->request_file == NULL)
		return -1;
	return 0;
}

