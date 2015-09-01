#ifndef _ARP_H_
#define _ARP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <rte_timer.h>
#include <rte_ether.h>
#include <packet.h>
#include <common/common.h>
#include <netdev.h>

#define ARP_S_STALE 0x0
#define ARP_S_PROBE 0x1
#define ARP_S_COMPELTE 0x2 
#define ARP_S_DISCARD 0x8 


struct arp_node {
	unsigned state;
	be32 addr;
	struct ether_addr haddr;
	struct rte_timer timer;
	uint64_t update_time;
	struct net_device *ndev;
	TAILQ_HEAD(mbqueue, pktbuf) backlog;
	int (*sendpkt)(struct arp_node *node, struct rte_mbuf *mbuf);
};

void arp_rcv(struct rte_mbuf *mbuf, struct packet_type *pt);
int arp_send(struct net_device *ndev, uint16_t op, struct ether_addr *shaddr, be32 saddr, 
	struct ether_addr *dhaddr, be32 daddr);
int arp_init(struct net_device *ndev);
struct arp_node* arp_node_create(struct net_device *ndev, be32 addr, struct ether_addr *haddr, unsigned state);
struct arp_node *arp_node_lookup(struct net_device *ndev, be32 addr, int create);

#ifdef __clpusplus
}
#endif

#endif
