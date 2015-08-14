#include <sys/queue.h>
#include <rte_ether.h>
#include <rte_ethdev.h>

#include "packet.h"
#include "netif.h"

#define RTE_LOGTYPE_NETIF RTE_LOGTYPE_USER1+1 

static inline void
print_ether_addr(const char *what, struct ether_addr *addr)
{
    char buf[ETHER_ADDR_FMT_SIZE];
    ether_format_addr(buf, ETHER_ADDR_FMT_SIZE, addr);
    printf("%s%s", what, buf);
}

int 
netif_rx(struct rte_mbuf *mbuf, unsigned num)
{
	int retval = 0;
	unsigned i;
	struct rte_mbuf *mb;
	struct ether_hdr *eth_hdr;
	struct packet_type *pt;

	if (mbuf == NULL) {
		//RTE_LOG(INFO, NETIF, "mbuf is null!\n");
		return retval;
	}

	for (i = 0; i < num; i++) {
		mb = mbuf + i;
		eth_hdr = rte_pktmbuf_mtod(mb, struct ether_hdr *);
		LIST_FOREACH(pt, &ptype_base, list) {
			if (pt->type == eth_hdr->ether_type) {
				pt->func(mb, pt);
			}
		}
		printf("port:%u, packet_type:%u, ol_flags:%lx, ether_type:%u\n", 
                mb->port, mb->packet_type, mb->ol_flags, eth_hdr->ether_type);
		print_ether_addr("saddr=", &eth_hdr->s_addr);
		print_ether_addr("daddr=", &eth_hdr->d_addr);
		printf("\n");
	}

	return retval;
}

