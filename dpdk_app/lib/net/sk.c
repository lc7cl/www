
#include "sk.h"

struct sock* create_sock(__rte_unused int family, int proto, int mode, struct rte_ring *pipe)
{
	return inet_alloc_sock(proto, mode, pipe);
}

int sock_bind(struct sock* sk, uint32_t addr, uint16_t port)
{
	if (port == 0) {
		/*TODO select port*/
	}
	return sk->proto_ops->bind(sk, addr, port);
}


