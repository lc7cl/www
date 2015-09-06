
#include "nameserver.h"

#define MAX_SERVER_NB 256;

struct nameserver {
	char name[NAME_LENGTH_MAX];
	struct srvaddr_queue srvaddr_head;
	int nb;
	rte_rwlock_t rwlock;
};

static struct nameserver root = {
	.name = ".",
	.srvaddr_head = TAILQ_HEAD_INITIALIZER,
	.nb = 12,
	.rwlock = RTE_RWLOCK_INITIALIZER,
};

int _root_server_add(uint32_t addr)
{
	struct srvaddr *srv;

	TAILQ_FOREACH(srv, &root.srvaddr_head, list) {
		if (srv->addr == addr && srv->delete == 0)
			return 0;
	}

	srv = srvaddr_alloc(addr, 1);
	if (srv == NULL)
		return -1;

	TAILQ_INSERT_TAIL(&root.srvaddr_head, srv, list);
	root.nb++;
	return 0;
}

void _root_server_delete(uint32_t addr)
{
	struct srvaddr *srv = NULL;

	TAILQ_FOREACH(srv, &root.srvaddr_head, list) {
		if (srv->addr == addr && srv->delete == 0)
			break;
	}

	if (srv) {
		TAILQ_REMOVE(&root.srvaddr_head, srv, list);
		root.nb--;
		srvaddr_free(srv);
	}
}

int root_server_add(uint32_t addr)
{
	int ret;
	
	rte_rwlock_write_lock(&root->rwlock);
	ret = _root_server_add(addr);
	rte_rwlock_write_unlock(&root->rwlock);
	return ret;
}

int root_server_add_bulk(uint32_t *addr, int nb)
{
	int i, ret;

	rte_rwlock_write_lock(&root->rwlock);
	for (i = 0; i < nb, i++) {
		ret = _root_server_add(addr);
		if (ret)
			goto fail;
	}
	rte_rwlock_write_unlock(&root->rwlock);
	return 0;
fail:
	for (i; i != 0; i--) {
		_root_server_delete(addr[i - 1]);
	}	
	rte_rwlock_write_unlock(&root->rwlock);
	return -1;
}

void root_server_delete(uint32_t addr) 
{
	rte_rwlock_write_lock(&root->rwlock);
	_root_server_delete(addr);
	rte_rwlock_write_unlock(&root->rwlock);
}

void root_server_delete_bulk(uint32_t *addr, int nb)
{
	int i;
	
	rte_rwlock_write_lock(&root->rwlock);
	for (i = 0; i < nb; i++) {
		_root_server_delete(addr[i]);
	}	
	rte_rwlock_write_unlock(&root->rwlock);
}

struct srvaddr* root_server_lookup(uint32_t addr, int add)
{
	int i, found = 0;
	struct srvaddr *srv = NULL, *retval = NULL;

	rte_rwlock_read_lock(&root->rwlock);
	TAILQ_FOREACH(retval, &root->srvaddr_head, list) {
		if (retval->delete == 0
			&& retval->addr == addr) {
			rte_atomic32_inc(&retval->refcnt);
			break;
		}
	}
	rte_rwlock_read_unlock(&root->rwlock);

	if (retval == NULL && add) {
		retval = srvaddr_alloc(addr, 1);
		if (retval == NULL)
			return NULL;
		rte_rwlock_write_lock(&root->rwlock);
		TAILQ_FOREACH(srv, &root.srvaddr_head, list) {
			if (srv->addr == addr && srv->delete == 0) {
				retval = srv;
				found = 1;
				break;
			}
		}
		if (found == 0) {
			TAILQ_INSERT_TAIL(&root.srvaddr_head, retval, list);
			root.nb++;
		}
		rte_atomic32_inc(&retval->refcnt);		
		rte_rwlock_write_unlock(&root->rwlock);
	}
	return retval;
}

struct tld* tld_create(char *name)
{
	struct tld *new;

	if (name == NULL) 
		return NULL;

	new = rte_malloc(struct tld);
	if (new == NULL)
		return NULL;

	strncpy(new->name, name, NAME_LENGTH_MAX);
	TAILQ_INIT(&new->srvaddr_head);
	new->nb = 0;
	rte_rwlock_init(&new->rwlock);
	return new;
}

int tld_add_server(struct tld *tld, uint32_t addr, int permanent)
{
	struct srvaddr *srv;

	rte_rwlock_write_lock(&tld->rwlock);
	TAILQ_FOREACH(srv, &tld->srvaddr_head, list) {
		if (srv->addr == addr && srv->delete == 0) {
			if (srv->permanent == 0 && permanent)
				srv->permanent = permanent;
			rte_rwlock_write_unlock(&tld->rwlock);
			return 0;
		}
	}	
	srv = rte_malloc(NULL, sizeof(struct srvaddr), 0);
	if (srv == NULL) {
		rte_rwlock_write_unlock(&tld->rwlock);
		return -1;
	}
	TAILQ_INSERT_TAIL(&tld->srvaddr_head, srv, list);
	tld->nb++;
	rte_rwlock_write_unlock(&tld->rwlock);
	return 0;
}

void tld_delete_server(struct tld *tld, uint32_t addr) 
{
	struct srvaddr *srv = NULL;

	rte_rwlock_write_lock(&tld->rwlock);
	TAILQ_FOREACH(srv, &tld->srvaddr_head, list) {
		if (srv->addr == addr && srv->delete == 0) {
			if (srv->permanent == 0) 
				TAILQ_REMOVE(&tld->srvaddr_head, srv, list);
			rte_rwlock_write_unlock(&tld->rwlock);
			return 0;
		}
	}
	rte_rwlock_write_unlock(&tld->rwlock);
}

struct srvaddr* tld_lookup_server(struct tld *tld, uint32_t addr)
{
	struct srvaddr *srv = NULL;

	rte_rwlock_read_lock(&tld->rwlock);
	TAILQ_FOREACH(srv, &tld->srvaddr_head, list) {
		if (srv->addr == addr && srv->delete == 0) {
			rte_atomic32_inc(&srv->refcnt);			
			rte_rwlock_read_unlock(&tld->rwlock);
			return srv;
		}
	}
	rte_rwlock_read_unlock(&tld->rwlock);
	return NULL;
}
