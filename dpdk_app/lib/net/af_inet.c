
#include "packet.h"
#include "udp.h"
#include "tcp.h"
#include "ipv4.h"
#include "af_inet.h"

static struct net_protocol *inet_protos[MAX_INET_PROTOS];

static struct net_protocol udp_protocol = {
	.protocol = UDP_ID,
	.handler = udp_rcv
};

static struct net_protocol tcp_protocol = {
	.protocol = TCP_ID,
	.handler = tcp_rcv
};

int inet_add_protocol(struct net_protocol *protocol)
{
	if (protocol == NULL) {
		return -1;
	}

	inet_protos[protocol->protocol] = protocol;

	return 0;
}

int inet_proto_register(void)
{
	int retval = 0;

	return retval;
}

int inet_init(void)
{
	int retval;

	retval = udp_init();
	
	retval = packet_init();
	retval = inet_add_protocol(&udp_protocol);
	retval = inet_add_protocol(&tcp_protocol);
	return retval;
}
