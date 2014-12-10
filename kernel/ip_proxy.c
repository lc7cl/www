#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/bitmap.h>

#include "ip_proxy.h"

static struct list_head client_list;
static DEFINE_SPINLOCK(spinlock); 

static unsigned long *bitmap;
unsigned short base_port;

int find_client(unsigned int saddr, unsigned short sport, struct proxy_client** client)
{
	struct proxy_client *p;

	if (!client) 
		return 0;

	rcu_read_lock();
	list_for_each_entry_rcu(p, &client_list, list) {
		if (p->saddr == saddr && p->sport == sport) {
			*client = p;
			rcu_read_unlock();
			return p->id;
		}
	}
	rcu_read_unlock();
	return 0;
}

int find_client_by_port(unsigned short port, struct proxy_client **client)
{
	struct  proxy_client *entry;

	if (port < base_port) 
		return 0;

	if (!client)
		return 0;

	rcu_read_lock();
	list_for_each_entry_rcu(entry, &client_list, list) {
		if (entry->id == (port - base_port + 1)) {
			*client = entry;
			rcu_read_unlock();
			return 1;
		}
	}
	rcu_read_unlock();
	return 0;
}

int add_client(unsigned int addr, unsigned short port)
{
	struct proxy_client *new;

	if (!addr || !port)
		return -1;

	if (find_client(addr, port, &new))
		return -1;

	new = kzalloc(sizeof(*new), GFP_KERNEL);
	if (!new)
		return -1;

	new->saddr = addr;
	new->sport = port;
	new->id = find_first_zero_bit(bitmap, BITS_TO_LONGS(256));
	INIT_LIST_HEAD(&new->list);
	spin_lock_init(&new->lock);

	spin_lock(&spinlock);
	list_add_rcu(&client_list, &new->list);
	spin_unlock(&spinlock);

	printk("new_id:%d\n", new->id);
	return new->id;
}

int remove_client(struct proxy_client *client)
{
	struct proxy_client *entry = client;

	if (!entry)
		return -1;
	
	list_del_rcu(&entry->list);
	synchronize_rcu();
	
	kfree(entry);

	return 0;
}

int __must_check proxy_init(void) 
{
	bitmap = kzalloc(sizeof(long) * BITS_TO_LONGS(256), GFP_KERNEL);
	if (!bitmap)
		return -1;

	INIT_LIST_HEAD(&client_list);
	spin_lock_init(&spinlock);
	base_port = 12345;

	return 0;
}

void proxy_fini(void) 
{
	if (!bitmap)
		kfree(bitmap);
}

