#include <rte_ether.h>

#include "af_inet.h"
#include "ipv4.h"
#include "packet.h"

struct ptype_list ptype_base;

int 
packet_type_add(struct packet_type *pt)
{
	LIST_INSERT_HEAD(&ptype_base, pt, list);
	return 0;
}

struct packet_type ipv4_packet = {
	.type = ETHER_TYPE_IPv4,
	.func = ipv4_rcv,
	.list = LIST_HEAD_INITIALIZER(NULL)
};

int packet_xmit(int port, struct rte_mbuf *mbuf)
{
	struct ether_hdr *eth_hdr;
		
	NET_ASSERT(dev != NULL && mbuf != NULL);
	
	rte_pktmbuf_prepend(mbuf, sizeof(struct ether_hdr));
	eth_hdr = rte_pktmbuf_mtod(mbuf, struct ether_hdr *);
	rte_eth_macaddr_get(&eth_hdr->s_addr);
	/*TODO arp*/
	//rte_eth_macaddr_get(&eth_hdr->d_addr);
	eth_hdr->ether_type = ETHER_TYPE_IPv4;
	return dev->ops->xmit(dev, mbuf);
}

int 
packet_init(void) 
{
	int retval;

	retval = packet_type_add(&ipv4_packet);
	
	return retval;
}
	
