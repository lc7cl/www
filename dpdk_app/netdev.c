#include <sys/queue.h>
#include <rte_malloc.h>
#include <rte_ether.h>
#include <rte_ethdev.h>

#include "netdev.h"

struct net_device dev_array[RTE_MAX_ETHPORTS];

#define DEFAULT_NDEV_NAME "eth%u"

enum {
    DEV_DETACHED = 0,
    DEV_ATTACHED
};

struct net_device_ops default_ndev_ops = {
	NULL, /*net_dev_add_v4addr*/
	NULL, /*net_dev_add_v4addrs*/
	NULL, /*net_dev_del_v4addr*/
	NULL, /*net_dev_del_v4addrs*/
	NULL, /*net_dev_get_primary_v4addr*/
	NULL, /*net_dev_get_v4addrs*/
	NULL, /*net_dev_set_mode*/
	NULL, /*net_dev_enable*/
	NULL, /*net_dev_start*/
	NULL, /*net_dev_stop*/
};

/**
*
*/
struct net_device* net_device_alloc(unsigned portid,
	char *name, struct net_device_ops *ops)
{
	struct net_device *dev;
	struct rte_eth_dev *eth_dev;
	size_t name_len;
	
	if (!rte_eth_dev_is_valid_port(portid)) {
		RTE_LOG(WARNING, NET, "%s %d invalid portid %u !\n", 
			__func__, __LINE__, portid);
		return NULL;
	}

	dev = &dev_array[portid];
	if (dev->dev != NULL) {
		/*TODO*/
		RTE_LOG(WARNING, NET, "%s %d portid %u is used!\n"
			, __func__, __LINE__, portid);
		return NULL;
	}

	if (name == NULL)
		name = DEFAULT_NDEV_NAME;

	name_len = strnlen(name, MAX_NIC_NAME_SIZE);
	if (name_len == 0 || name_len == MAX_NIC_NAME_SIZE) {
		RTE_LOG(WARNING, NET, "%s %d invalid device name!\n", __func__, __LINE__);
		return NULL;
	}

	if (ops == NULL)
		ops = &default_ndev_ops;

	dev->dev = eth_dev;
	dev->ops = ops;
	dev->portid = portid;
	dev->flag = NET_DEV_F_ENABLE;
	strncpy(dev->name, name, name_len);
	dev->v4_addr = NULL;

	return dev;	
}


/**
*
*/
void net_device_release(unsigned portid)
{

}


/**
*
*/
int net_dev_ctrl(struct net_device *dev)
{
}

int net_device_init(unsigned *portid, int length)
{
	int retval, i;

	if (portid == NULL || length == 0)
		return -1;

	for (i = 0; i < length; i++) {
		retval = net_device_alloc(portid[i], NULL, NULL);
		if (retval == NULL)
			goto error_release_ndev;
	}
	return 0;
error_release_ndev:
	for (i; i >= 0; i--) {
		net_device_release(portid[i]);
	}
	return -1;
}
