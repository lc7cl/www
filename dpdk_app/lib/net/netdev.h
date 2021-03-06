#ifndef _NETDEV_H_
#define _NETDEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <rte_ether.h>
#include <rte_hash.h>
#include "common/common.h"
#include "buffer.h"
#include <arp.h>

#define MAX_NET_DEVICE_COUNT 64

#define NET_DEV_F_NONE         0x0
#define NET_DEV_F_ENABLE       0x00000001
#define NET_DEV_F_DISABLE      0x00000002
#define NET_DEV_F_START        0x00000004
#define NET_DEV_F_STOP         0x00000008
#define NET_DEV_F_LOOPBACK     0x00000010
#define NET_DEV_F_PROMISCUOUS  0x00001000

enum {
	NDEV_CTRL_T_INET_ADDR,
	NDEV_CTRL_T_DEV_NAME,
};

enum {
	INET_ADD_V4ADDR = 0,
	INET_DEL_V4ADDR
};

enum {
	NAME_SET = 0,
	NAME_GET
};


#define IS_LOOPBACK_DEVICE(dev) \
	(((dev)->flag & NET_DEV_F_LOOPBACK) == NET_DEV_F_LOOPBACK)

#define IPV6_ADDRESS_LENGTH 16
struct ip_addr {
	union {
		be32 ipv4;
		uint8_t ipv6[IPV6_ADDRESS_LENGTH];
	} addr;
	uint8_t mask;
	struct ip_addr *next;
};

struct net_device;

struct net_device_ops {
	int (*add_v4addr)(struct net_device *dev, struct iovec *pstr);
	int (*add_v4addrs)(struct net_device *dev, struct iovec *ppstr, int count);
	int (*del_v4addr)(struct net_device *dev, struct iovec *pstr);
	int (*del_v4addrs)(struct net_device *dev, struct iovec *ppstr, int count);
	int (*get_primary_v4addr)(struct net_device *dev, __out uint32_t *v4addr);
	int (*get_v4addrs)(struct net_device *dev, __out unsigned *count, 
		__out uint32_t *v4addr, int length);
	int (*set_mode)(struct net_device *dev, unsigned mode);
	int (*enable)(struct net_device *dev, int enable);
	int (*start)(struct net_device *dev);
	int (*stop)(struct net_device *dev);
	int (*set_name)(struct net_device *dev, struct iovec *pstr);
	int (*get_name)(struct net_device *dev, struct iovec *pstr);
	int (*xmit)(struct net_device *dev, struct rte_mbuf *mb);
};

#define MAX_NIC_NAME_SIZE  64
typedef struct net_device {
	char name[MAX_NIC_NAME_SIZE];
	unsigned portid;
	unsigned flag;
	struct ether_addr haddr;
	struct ip_addr *v4_addr;
	struct net_device_ops *ops;
	struct rte_hash *arp_table;
	struct rte_mempool *arp_mbuf_mp;
	struct {
		struct {
			uint64_t drop[RTE_MAX_LCORE];
			uint64_t xmit[RTE_MAX_LCORE];
		} tx;
		struct {			
			uint64_t drop[RTE_MAX_LCORE];
			uint64_t recv[RTE_MAX_LCORE];
		} rx;
	} stat;
} net_device_t;


extern struct net_device dev_array[];

/**
* param portid:
* return :
*/
static inline struct net_device* net_device_get(unsigned portid)
{
	struct net_device *ndev;
	if (portid >= RTE_MAX_ETHPORTS)
		return NULL;
	ndev = &dev_array[portid];
	if (ndev->flag != NET_DEV_F_NONE
		&& (ndev->flag & NET_DEV_F_ENABLE))
		return ndev;
	return NULL;
}

static inline be32 net_device_get_primary_addr(unsigned portid)
{
	struct net_device *ndev;
	struct ip_addr* ip_addr;
	
	ndev = net_device_get(portid);
	if (ndev == NULL)
		return 0;

	ip_addr = ndev->v4_addr;
	if (ip_addr) {
		return ip_addr->addr.ipv4;
	}
	
	return 0;
}

struct net_device* net_device_alloc(unsigned portid,
	const char *name, struct net_device_ops *ops);

int net_dev_ctrl(struct net_device *dev, int ctrl_type, struct msg_hdr *param);

int net_device_init(unsigned portid);
void net_device_release(__rte_unused unsigned portid);
int net_device_inet_addr_match(struct net_device *ndev, be32 ipv4_addr);

#ifdef __cplusplus
}
#endif

#endif
