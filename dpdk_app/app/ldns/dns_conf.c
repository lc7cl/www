#include "dns_conf.h"

static struct dns_conf *gcfg;

int dns_set_cfg(struct dns_conf *cfg)
{
	gcfg = cfg;
	return 0;
}

struct dns_conf* dns_get_cfg()
{
	return gcfg;
}

