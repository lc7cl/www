
#include "packet.h"
#include "sk.h"
#include "udp.h"
#include "tcp.h"
#include "ipv4.h"
#include "common/common.h"
#include "af_inet.h"

#define NB_AF_INET_SOCK_NUM 65536
static struct rte_mempool *af_inet_sock_pool;

struct net_protocol *inet_protos[MAX_INET_PROTOS];

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

static struct proto *sock_proto[SOCK_PTOTO_IPPROTO_MAX];

int inet_proto_register(void)
{
	int retval = 0;

	sock_proto[SOCK_PTOTO_IPPROTO_UDP] = &udp_proto;

	return retval;
}

int inet_init(void)
{
	af_inet_sock_pool = rte_mempool_create("AF_INET_SOCK",
		NB_AF_INET_SOCK_NUM,
		sizeof(struct sock),
		32,//unsigned cache_size,
		0,//unsigned private_data_size,
		NULL,//rte_mempool_ctor_t * mp_init,
		NULL,//void * mp_init_arg,
		NULL,//rte_mempool_obj_ctor_t * obj_init,
		NULL,//void * obj_init_arg,
		SOCKET_ID_ANY,//int socket_id,
		0);

	if (af_inet_sock_pool == NULL)
		return -1;

	if (inet_proto_register())
		return -1;

	if (packet_init())
		return -1;

	if (inet_add_protocol(&udp_protocol))
		return -1;
	
	if (inet_add_protocol(&tcp_protocol))
		return -1;

	if (udp_init())
		return -1;
	
	return 0;
}

struct sock *inet_alloc_sock(int proto, struct sock_parameter *param)
{
	struct sock *sk;

	NET_ASSERT(af_inet_sock_pool != NULL && param != NULL);

	if (rte_mempool_get(af_inet_sock_pool, (void **)&sk))
		return NULL;

	if (param->mode == SOCK_MODE_COMPLETE) {
		if (param->func == NULL)
			goto destroy_sock;
		else
        	sk->param.func = param->func;
	} else if (param->mode == SOCK_MODE_PIPLINE) {
		if (param->pipe == NULL)
			goto destroy_sock;
		else
			sk->param.pipe = param->pipe;
	} else {
		RTE_LOG(WARNING, PROTO, "mode is unsupported\n");
		goto destroy_sock;
	}
	sk->param.mode = param->mode;
	if (proto >= SOCK_PTOTO_IPPROTO_MAX) {
		RTE_LOG(WARNING, PROTO, "wrong proto\n");
		goto destroy_sock;
	}
	sk->l4_proto = proto;
	sk->proto_ops = sock_proto[proto];
	if (sk->proto_ops == NULL) {
		RTE_LOG(WARNING, PROTO, "proto is unsupported\n");
		goto destroy_sock;
	}
	sk->next_ip_id = 0;
	sk->nic = RTE_MAX_ETHPORTS;

	return sk;

destroy_sock:
	rte_mempool_put(af_inet_sock_pool, sk);
	return NULL;
}
