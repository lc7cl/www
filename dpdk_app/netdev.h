#ifndef _NETDEV_H_
#define _NETDEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common/common.h"

#define MAX_NET_DEVICE_COUNT 64

#define NET_DEV_F_ENABLE       0x00000001
#define NET_DEV_F_DISABLE      0x00000002
#define NET_DEV_F_START        0x00000004
#define NET_DEV_F_STOP         0x00000008

#define NET_DEV_F_PROMISCUOUS 0x00000100

#define IPV6_ADDRESS_LENGTH 16
struct ip_addr {
	union {
		uint32_t ipv4;
		uint8_t ipv6[IPV6_ADDRESS_LENGTH];
	} addr;
	struct ip_addr *next;
};

struct net_device;

struct net_device_ops {
	int (*net_dev_add_v4addr)(struct net_device *dev, uint32_t v4addr);
	int (*net_dev_add_v4addrs)(struct net_device *dev, uint32_t v4addrs, int length);
	int (*net_dev_del_v4addr)(struct net_device *dev, uint32_t v4addr);
	int (*net_dev_del_v4addrs)(struct net_device *dev, uint32_t *v4addr, int length);
	int (*net_dev_get_primary_v4addr)(struct net_device *dev, __out uint32_t *v4addr);
	int (*net_dev_get_v4addrs)(struct net_device *dev, __out unsigned *count, 
		__out uint32_t *v4addr, int length);
	int (*net_dev_set_mode)(struct net_device *dev, unsigned mode);
	int (*net_dev_enable)(struct net_device *dev, int enable);
	int (*net_dev_start)(struct net_device *dev);
	int (*net_dev_stop)(struct net_device *dev);
};

#define MAX_NIC_NAME_SIZE  64
typedef struct net_device {
	char name[MAX_NIC_NAME_SIZE];
	unsigned portid;
	unsigned flag;
	struct ip_addr *v4_addr;
	struct net_device_ops *ops;
	struct rte_eth_dev *dev;
} net_device_t;


extern struct net_device dev_array[];

/**
* param portid:
* return :
*/
static inline struct net_device* net_device_get(unsigned portid)
{
	if (portid >= RTE_MAX_ETHPORTS)
		return NULL;
	return &dev_array[portid];
}

struct net_device* net_device_alloc(unsigned portid,
	char *name, struct net_device_ops *ops);

#ifdef __cplusplus
}
#endif

#endif
