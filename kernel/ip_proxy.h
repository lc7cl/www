#ifndef IP_PROXY_H
#define IP_PROXY_H

#include <linux/rculist.h>

struct proxy_client {
	unsigned char id;
	unsigned int saddr;
	unsigned short sport;
	struct list_head list;
	spinlock_t lock;
};

extern unsigned short base_port;

extern int remove_client(struct proxy_client *client);
extern int __must_check proxy_init(void);
extern void proxy_fini(void);
extern int add_client(unsigned int addr, unsigned short port);
extern int find_client_by_port(unsigned short port, struct proxy_client **client);
extern int find_client(unsigned int saddr, unsigned short sport, struct proxy_client** client);

#endif
