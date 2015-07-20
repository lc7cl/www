#include <rte_ether.h>
#include <rte_ethdev.h>

#include "port.h"

#define RTE_LOGTYPE_PORT RTE_LOGTYPE_USER1+2

#define MAX_PORT_NUM 32

static struct port* portlist[MAX_PORT_NUM];

int 
port_init(unsigned port_id)
{
	int retval;

	return retval;
}

int
port_register(unsigned port_id)
{
	int retval = -1;

	if (pconf == NULL)
		return retval;

	memset(pconf, 0, sizeof(pconf));
	if (port_id >= MAX_PORT_NUM) {
		RTE_LOG(INFO, PORT, "port num excceds %d\n", MAX_PORT_NUM);
		return retval;
	}

	portlist[port_id] = pconf;	
}

