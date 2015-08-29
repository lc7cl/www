#ifndef _ARP_H_
#define _ARP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <rte_ether.h>
#include <packet.h>
#include <common/common.h>

void arp_rcv(struct rte_mbuf *mbuf, struct packet_type *pt);
int arp_send(int op, struct ether_hdr *shaddr, be32 saddr, 
	struct ether_hdr *dhaddr, be32 daddr);
int arp_init(void);



#ifdef __clpusplus
}
#endif

#endif
