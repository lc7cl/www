#include "packet_construct.h"

int get_request(struct port_conf *port_cfg, struct request_date *req)
{
#define LINE_SIZE 1024
	FILE *f;
	struct rte_ring *ring;
	char *file_path;
	char line[LINE_SIZE];
	char domain[LINE_SIZE];
	int domain_len;
	char *pch;
	struct request_data *req;
	int ret;

	f = fopen(port_cfg->file_path, "r");
	if (f == NULL) {
		printf("cannot open file %s\n", file_path);
		return;
	}

	while (1) {		
		while (fgets(line, LINE_SIZE, f)) {
			domain_len = 0;
			pch = strtok(line, " \n");		
			if (pch == NULL) {
				continue;
			} else {
				strcpy(domain, pch);
				domain_len = strnlen(domain, LINE_SIZE) + 1 + 1;
			}
			pch = strtok(NULL, " \n");
			if (pch == NULL) {
				continue;
			} else if (strcmp(pch, "A") == 0) { 
				if (format_domain(domain, req->req_domain, 512) == 0) {
					rte_free(req);
					continue;
				}
				req->size = domain_len;
				req->req_type = DNS_QTYPE_A; 		
				ret = rte_ring_enqueue(ring, (void*)req);
				if (ret < 0)
					rte_free(req);
			} else if (strcmp(pch, "PTR") == 0) {
				req = rte_malloc_socket(NULL, sizeof(struct request_data) + domain_len, 0, rte_socket_id());
				if (req == NULL)
					break;
				if (format_domain(domain, req->req_domain, domain_len) == 0) {
					rte_free(req);
					continue;
				}
				req->size = domain_len;
				req->req_type = DNS_QTYPE_PTR;			
				ret = rte_ring_enqueue(ring, (void*)req);
				if (ret < 0)
					rte_free(req);
			} else {
				printf("%s %d %s %lu\n", __func__, __LINE__, line, strlen(pch));
			}	
		}
		
		if (feof(f)) {
			rewind(f);
		}
	}
    return 0;
}
