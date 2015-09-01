#ifndef _PACKET_H_
#define _PACKET_H_

#include <sys/queue.h>

#include <rte_mbuf.h>

typedef struct packet_type {
	unsigned type;
	void (*func)(struct rte_mbuf *mbuf, struct packet_type *pt);
	LIST_ENTRY(packet_type) list;
} packet_type_t;

LIST_HEAD(ptype_list, packet_type);

extern struct ptype_list ptype_base;

int packet_type_add(struct packet_type *pt);
int packet_init(void);
int packet_xmit(unsigned port, struct rte_mbuf *mbuf);

#endif
