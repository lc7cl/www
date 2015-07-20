#ifndef _PACKET_H_
#define _PACKET_H_

typedef struct packet_type {
	unsigned type;
	void (*func)(struct rte_mbuf *mbuf, struct packet_type *pt);
	LIST_ENTRY(packet_type) list;
} packet_type_t;

int packet_type_add(struct packet_type *pt);


#endif
