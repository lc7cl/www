#include "client.h"
#include "dispatch.h"

int process_client_request()
{
    return 0;
}

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
        process_client_request();
    } else {
        /*dns response*/
        process_server_response();
    }
    printf("%u  %u\n", addr, port);

    return 0;
}
