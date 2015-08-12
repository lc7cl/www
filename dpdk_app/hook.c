#include <rte_rwlock.h>

#include "hook.h"

static LIST_HEAD(hook_head, hook_ops) hhead[HOOK_PROTO_MAX][HOOK_POS_MAX];

static rte_rwlock_t hook_lck;

int hook_register(struct hook_ops *ops)
{
	if (ops == NULL
		|| ops->proto >= HOOK_PROTO_MAX
		|| ops->pos >= HOOK_POS_MAX
		|| ops->func == NULL)
		return -1;

	rte_rwlock_write_lock(&hook_lck);
	LIST_INSERT_HEAD(&hhead[ops->proto][ops->pos], ops, list);
	rte_rwlock_write_unlock(&hook_lck);
}

int hook_unregister(struct hook_ops *ops)
{
	if (ops == NULL
		|| ops->proto >= HOOK_PROTO_MAX
		|| ops->pos >= HOOK_POS_MAX
		|| ops->func == NULL)
		return -1;

	rte_rwlock_write_lock(&hook_lck);
	LIST_REMOVE(ops, list);
	rte_rwlock_write_unlock(&hook_lck);
}


