#ifndef _AF_INET_H_
#define _AF_INET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <rte_ip.h>

#define MAX_INET_PROTOS             256
#define IPV4_VERSION_MASK           0xf0
#define IPV4_HEADLENGTH_MASK        0x0f
#define UDP_ID                      17
#define TCP_ID                      6
#define ICMP_ID                     1

#define IS_BROADCAST_IPV4(v4addr, mask)\
	((v4addr != (v4addr & ((1 << mask) - 1) << (32 - mask))) \
	&& (v4addr != (v4addr || ((1 << (32 - mask)) - 1))))

#define IS_VALID_UC_IPV4(dev, v4addr, mask)\
	((IS_LOOPBACK_DEVICE(dev) && (v4addr == IPV4_LOOPBACK))\
	|| (!IS_BROADCAST_IPV4(v4addr, mask) && !IS_IPV4_MCAST(v4addr)))

struct net_protocol {
	uint8_t protocol;
	void (*handler)(struct rte_mbuf*, struct ipv4_hdr*);
};

extern struct net_protocol *inet_protos[];

int inet_add_protocol(struct net_protocol *protocol);
int inet_proto_register(void);
int inet_init(void);

struct sock;
struct sock_parameter;
struct sock *inet_alloc_sock(int proto, struct sock_parameter *param);


#ifdef __cplusplus
}
#endif

#endif
