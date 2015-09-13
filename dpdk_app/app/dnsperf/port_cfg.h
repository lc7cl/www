#ifndef _PORT_CFG_H_
#define _PORT_CFG_H_

#include <rte_mempool.h>

enum port_mode {
	PORT_MODE_SINGLE = 0,
	PORT_MODE_RANGE,
};

struct request_data {
	struct request_data *next;
	uint16_t type;
	
	char domain[0];		
};

struct single_pkt_info {
	uint16_t dst_port;
	uint16_t src_port;		
	uint32_t dst_addr;
	uint32_t src_addr;
	struct ether_addr dst_haddr;
	struct ether_addr src_haddr;
	
};

struct range_pkt_info {
	uint16_t dst_port;
	uint16_t src_port;		
	uint32_t dst_addr_a;
	uint32_t dst_addr_b;
	uint32_t src_addr_a;
	uint32_t src_addr_b;
	struct ether_addr dst_haddr;
	struct ether_addr src_haddr;	
};

struct port_conf {
	int mode;
	char *reqest_file;	
	struct request_data *req_head;
	struct request_data *req_tail;
	struct rte_mempool *pool1;
	struct rte_mempool *pool2;
	struct single_pkt_info single_pkt;
	struct range_pkt_info range_pkt;
};

static inline void single_pkt_set_dst_port(struct port_conf *cfg, uint16_t dst_port)
{
	cfg->single_pkt.dst_port = dst_port;
}

static inline void single_pkt_set_src_port(struct port_conf *cfg, uint32_t src_port)
{
	cfg->single_pkt.src_port = src_port;
}

static inline void single_pkt_set_dst_addr(struct port_conf *cfg, uint32_t dst_addr)
{
	cfg->single_pkt.dst_addr = dst_addr;
}

static inline void single_pkt_set_src_addr(struct port_conf *cfg, uint32_t src_addr)
{
	cfg->single_pkt.src_addr = src_addr;
}

static inline void single_pkt_set_dst_haddr(struct port_conf *cfg, struct ether_addr *haddr)
{
	ether_addr_copy(haddr, &cfg->single_pkt.dst_haddr);
}

static inline void single_pkt_set_src_haddr(struct port_conf *cfg, struct ether_addr *haddr)
{
	ether_addr_copy(haddr, &cfg->single_pkt.src_haddr);
}

static inline void range_pkt_set_dst_port(struct port_conf *cfg, uint16_t dst_port)
{
	cfg->range_pkt.dst_port = dst_port;
}

static inline void range_pkt_set_src_port(struct port_conf *cfg, uint32_t src_port)
{
	cfg->range_pkt.src_port = src_port;
}


static inline void range_pkt_set_dst_addr(struct port_conf *cfg, uint32_t addr_a, uint32_t addr_b)
{
	cfg->range_pkt.dst_addr_a = addr_a;
	cfg->range_pkt.dst_addr_b = addr_b;
}

static inline void range_pkt_set_src_addr(struct port_conf *cfg, uint32_t addr_a, uint32_t addr_b)
{
	cfg->range_pkt.dst_addr_a = addr_a;
	cfg->range_pkt.dst_addr_b = addr_b;
}

static inline void range_pkt_set_dst_haddr(struct port_conf *cfg, struct ether_addr *haddr)
{
	ether_addr_copy(haddr, &cfg->range_pkt.dst_haddr);
}

static inline void range_pkt_set_src_haddr(struct port_conf *cfg, struct ether_addr *haddr)
{
	ether_addr_copy(haddr, &cfg->range_pkt.src_haddr);
}

int port_add_request_domain(struct port_conf *cfg, char *domain, int type);
int port_add_request_file(struct port_conf *cfg, char *file);


#endif
