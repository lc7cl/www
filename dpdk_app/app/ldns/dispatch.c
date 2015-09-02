#include "client.h"
#include "dispatch.h"

int dispatch_dns_pkt(struct rte_mbuf *mbuf, uint32_t addr, uint16_t port)
{
    struct dns_hdr *dnshdr;

    dnshdr = rte_pktmbuf_mtod(mbuf, struct dns_hdr*);
    if (dnshdr->qr == 0) {
        /*dns request*/
    } else {
        /*dns response*/
    }
    printf("%u  %u\n", addr, port);

    return 0;
}
