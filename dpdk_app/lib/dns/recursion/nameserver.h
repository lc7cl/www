#ifndef _NAMESERVER_H_
#define _NAMESERVER_H_

#include <rte_rwlock.h>

#include <common/dns.h>
#include <srvaddr.h>

struct tld {
	char name[NAME_LENGTH_MAX];
	struct srvaddr_queue srvaddr_head;
	int nb;
	rte_rwlock_t rwlock;
};

int root_server_add(uint32_t addr);
int root_server_add_bulk(uint32_t *addr, int nb);
void root_server_delete(uint32_t addr);
void root_server_delete_bulk(uint32_t *addr, int nb);
struct srvaddr* root_server_lookup(uint32_t addr, int add);

struct tld* tld_create(char *name);
int tld_add_server(struct tld *tld, uint32_t addr, int permanent);
void tld_delete_server(struct tld *tld, uint32_t addr);
struct srvaddr* tld_lookup_server(struct tld *tld, uint32_t addr);

#endif
