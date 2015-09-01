#ifndef _ARP_H_
#define _ARP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <rte_timer.h>
#include <rte_ether.h>
#include <packet.h>
#include <common/common.h>

#define ARP_S_STALE 0x0
#define ARP_S_PROBE 0x1
#define ARP_S_COMPELTE 0x2 

struct arp_node {
	unsigned state;
	be32 addr;
	struct ether_addr haddr;
	struct rte_timer timer;
	uint64_t update_time;
	TAILQ_HEAD(mbqueue, pktbuf) backlog;
};

void arp_rcv(struct rte_mbuf *mbuf, struct packet_type *pt);
int arp_send(int op, struct ether_hdr *shaddr, be32 saddr, 
	struct ether_hdr *dhaddr, be32 daddr);
int arp_init(void);



#ifdef __clpusplus
}
#endif

#endif
