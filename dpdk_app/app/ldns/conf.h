#ifndef _CONF_H_
#define _CONF_H_

enum {
	DNS_MODE_RECURSIVE_ONLY,
	DNS_MODE_L1_CACHE,
	DNS_MODE_L2_CACHE,
	DNS_MODE_MIXED,
	DNS_MODE_AUTHORIZATION,
	DNS_MODE_MAX	
};

struct dns_conf {
	uint32_t mode;
	uint32_t remote_addr;
	char remote_mac[6];
};

int dns_set_cfg(struct dns_conf *cfg);
struct dns_conf* dns_get_cfg();

#endif
