#ifndef _PACKET_CONSTRUCT_H_
#define _PACKET_CONSTRUCT_H_

#include <rte_ether.h>
#include <rte_ip.h>
#include <rte_udp.h>

enum DNS_QTYPE {
	DNS_QTYPE_A 	= 1,
	DNS_QTYPE_NS 	= 2,
	DNS_QTYPE_MD	= 3,
	DNS_QTYPE_MF 	= 4,
	DNS_QTYPE_CNAME = 5,
	DNS_QTYPE_SOA 	= 6,
	DNS_QTYPE_MB 	= 7,
	DNS_QTYPE_MG 	= 8,
	DNS_QTYPE_MR 	= 9,
	DNS_QTYPE_NULL 	= 10,
	DNS_QTYPE_WKS 	= 11,
	DNS_QTYPE_PTR 	= 12,
	DNS_QTYPE_HINFO = 13,
	DNS_QTYPE_MINFO = 14,
	DNS_QTYPE_MX 	= 15,
	DNS_QTYPE_TXT 	= 16,
	DNS_QTYPE_AXFR 	= 252,
	DNS_QTYPE_MAILB	= 253,
	DNS_QTYPE_MAILA	= 254,
	DNS_QTYPE_ALL	= 255
};

enum DNS_CLASS {
	DNS_CLASS_IN 	= 1,
	DNS_CLASS_CS 	= 2,
	DNS_CLASS_CH 	= 3,
	DNS_CLASS_HS 	= 4,
	DNS_CLASS_ALL 	= 255
};

enum DNS_SECTION {
	SECTION_QUESTION,
	SECTION_ANSWER,
	SECTION_AUTHORITY,
	SECTION_ADDITIONAL,
};

struct dns_hdr {
	uint16_t id;
	uint8_t rd:1,
			tc:1,
			aa:1,
			opcode:4,
			qr:1;
	uint8_t rcode:4,
			z:3,
			ra:1;
	uint16_t qdcount;
	uint16_t ancount;
	uint16_t nscount;
	uint16_t arcount;			
} __attribute((__packed__))__;


static inline int ether_construct(struct ether_hdr *hdr, struct ether_addr *dst_mac, struct ether_addr *src_mac, uint16_t ether_type)
{
	ether_addr_copy(dst_mac, &hdr->d_addr);
	ether_addr_copy(src_mac, &hdr->s_addr);
	hdr->ether_type = rte_cpu_to_be_16(ether_type);
	return sizeof(struct ether_hdr);
}

static inline int ipv4_construct(struct ipv4_hdr *hdr, uint32_t dst_addr, uint32_t src_addr, 
	uint16_t id, uint16_t proto, uint16_t length, int nocsum)
{
	hdr->version_ihl = 0x45;
	hdr->type_of_service = 0;
	hdr->total_length = rte_cpu_to_be_16(length + sizeof(struct ipv4_hdr));
	hdr->packet_id = rte_cpu_to_be_16(id);
	hdr->fragment_offset = 0;
	hdr->time_to_live = 64;
	hdr->next_proto_id = proto;
	hdr->hdr_checksum = 0;
	hdr->src_addr = rte_cpu_to_be_32(src_addr);
	hdr->dst_addr = rte_cpu_to_be_32(dst_addr);
	if (nocsum == 0) {
		hdr->hdr_checksum = rte_ipv4_cksum(hdr);
	}
	return sizeof(struct ipv4_hdr);
}

static line int udp_construct(struct udp_hdr *hdr, uint16_t dport, uint16_t sport, 
	uint16_t length, const struct ipv4_hdr *ipv4_hdr)
{
	hdr->src_port = rte_cpu_to_be_16(sport);
	hdr->dst_port = rte_cpu_to_be_16(dport);
	hdr->dgram_len = rte_cpu_to_be_16(length + sizeof(struct udp_hdr));
	hdr->dgram_cksum = 0;
	hdr->dgram_cksum = rte_ipv4_udptcp_cksum(ipv4_hdr, hdr);
	return sizeof(struct udp_hdr);
}

int dns_request_construct(struct dns_hdr *hdr, char *domain, uint16_t type, uint16_t class, __rte_unused void* additional);

#endif
