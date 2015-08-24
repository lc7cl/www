
#include "sk.h"

struct sock* create_sock(__rte_unused int family, int proto, int mode, struct rte_ring *pipe)
{
	return inet_alloc_sock(proto, mode, pipe);
}


