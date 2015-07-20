#include <rte_ether.h>

#include "packet.h"
#include "ip.h"

static struct net_protocol {
	void (*handler)(struct rte_mbuf *mbuf);
} *inet_protos[MAX_INET_PROTOS];

int
ip_proto_register()
{
	int retval;

	return retval;
}

static void 
ip_rcv(struct rte_mbuf *mbuf, struct packet_type *pt)
{
	
}

static struct packet_type ip_packet = {
		.type = ETHER_TYPE_IPv4,
		.func = ip_rcv,
		.list = { NULL, NULL },
	};

int 
ip_init(void)
{
	int retval;
	
	retval = packet_type_add(&ip_packet);
	return retval;
}
