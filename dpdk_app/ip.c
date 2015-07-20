#include <rte_ether.h>

#include "packet.h"
#include "ip.h"

static void 
ip_recv(struct rte_mbuf *mbuf, struct packet_type *pt)
{
	
}

static struct packet_type ip_packet = {
		.type = ETHER_TYPE_IPv4,
		.func = ip_recv,
		.list = { NULL, NULL },
	};

int 
ip_init(void)
{
	int retval;
	
	retval = packet_type_add(&ip_packet);
	return retval;
}
