#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <rte_rwlock.h>
#include <common/dns.h>

struct client {
    uint32_t addr;
    uint16_t port;
    struct dns_question_queue q_list; 
    rte_atomic32_t refcnt;
    rte_rwlock_t rwlock;
};

struct client* client_lookup(uint32_t addr, uint16_t port, int create);
int client_add_question(struct dns_question *question);

#endif
