#include "client.h"
#include "dispatch.h"

static struct dns_mempool *mempool;

/*process dns request from stub dns*/
void process_client_request(struct rte_mbuf *mbuf, uint32_t addr, uint16_t port)
{
	struct dns_client *client;
	struct dns_query *query;
	struct dns_message *msg;
	int ret;

	client = dns_client_alloc(mempool->client_pool, addr, port);
	if (client == NULL) 
		return;

	if (rte_mempool_get(mempool->query_pool, &query) < 0) {
		dns_client_put(client);
		return;
	}

	if (rte_mempool_get(mempool->message_pool, &msg) < 0) { 
		dns_query_put(query);
		dns_client_put(client);
	}
	msg->pool = mempool;	
	ret = message_retrieve(mbuf, msg, mempool);
	if (ret != ESUCCESS)
		return;
	
	query->request = msg;
	dns_client_add_query(client, query);
	start_recursion(query);
}

/*process */
int process_server_response()
{
    return 0;
}

int dispatch_dns_pkt(struct rte_mbuf *mbuf, uint32_t addr, uint16_t port)
{
    struct dns_hdr *dnshdr;

    dnshdr = rte_pktmbuf_mtod(mbuf, struct dns_hdr*);
    if (dnshdr->qr == 0) {
        /*dns request*/
        process_client_request(mbuf, addr, port);
    } else {
        /*dns response*/
        process_server_response();
    }
    printf("%u  %u\n", addr, port);

    return 0;
}
