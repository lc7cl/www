#include "packet_construct.h"

int ether_construct(struct ether_hdr *hdr, struct ether_addr *dst_mac, struct ether_addr *src_mac, uint16_t ether_type)
{
	ether_addr_copy(dst_mac, &hdr->d_addr);
	ether_addr_copy(src_mac, &hdr->s_addr);
	hdr->ether_type = rte_cpu_to_be_16(ether_type);
	return sizeof(struct ether_hdr);
}

int udp_construct(struct udp_hdr *hdr, uint16_t dport, uint16_t sport, uint16_t len)
{
}

int dns_request_construct(struct dns_hdr *hdr, char *domain, uint16_t type, uint16_t class, __rte_unused void* additional)
{

}

