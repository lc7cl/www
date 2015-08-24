#ifndef _SK_H_
#define _SK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <rte_mbuf.h>
#include <rte_ring.h>

enum {
	SOCK_MODE_COMPELETE,
	SOCK_MODE_PIPLINE,
};

struct sock {
	void (*func)(struct rte_mbuf*);
	struct rte_ring backlog;
	struct rte_ring write_ring;
	unsigned lcore_id;
	int mode;	
	struct rte_ring* pipe_ring;
	int l4_proto;
	struct proto *proto_ops;
	struct l4_info {
		uint16_t port;
	} l4_info;
};

enum {
  SOCK_PTOTO_IPPROTO_IP = 0,		/* Dummy protocol for TCP		*/
#define SOCK_PTOTO_IPPROTO_IP 		SOCK_PTOTO_IPPROTO_IP
  SOCK_PTOTO_IPPROTO_ICMP = 1,		/* Internet Control Message Protocol	*/
#define SOCK_PTOTO_IPPROTO_ICMP 	SOCK_PTOTO_IPPROTO_ICMP
  SOCK_PTOTO_IPPROTO_IGMP = 2,		/* Internet Group Management Protocol	*/  
#define SOCK_PTOTO_IPPROTO_IGMP 	SOCK_PTOTO_IPPROTO_IGMP
  SOCK_PTOTO_IPPROTO_IPIP = 4,		/* IPIP tunnels (older KA9Q tunnels use 94) */
#define SOCK_PTOTO_IPPROTO_IPIP 	SOCK_PTOTO_IPPROTO_IPIP
  SOCK_PTOTO_IPPROTO_TCP = 6,		/* Transmission Control Protocol	*/
#define SOCK_PTOTO_IPPROTO_TCP 		SOCK_PTOTO_IPPROTO_TCP
  SOCK_PTOTO_IPPROTO_EGP = 8,		/* Exterior Gateway Protocol		*/  
#define SOCK_PTOTO_IPPROTO_EGP 		SOCK_PTOTO_IPPROTO_EGP
  SOCK_PTOTO_IPPROTO_PUP = 12,		/* PUP protocol				*/
#define SOCK_PTOTO_IPPROTO_PUP		SOCK_PTOTO_IPPROTO_PUP
  SOCK_PTOTO_IPPROTO_UDP = 17,		/* User Datagram Protocol		*/
#define SOCK_PTOTO_IPPROTO_UDP		SOCK_PTOTO_IPPROTO_UDP
  SOCK_PTOTO_IPPROTO_IDP = 22,		/* XNS IDP protocol			*/
#define SOCK_PTOTO_IPPROTO_IDP		SOCK_PTOTO_IPPROTO_IDP
  SOCK_PTOTO_IPPROTO_TP = 29,		/* SO Transport Protocol Class 4	*/
#define SOCK_PTOTO_IPPROTO_TP		SOCK_PTOTO_IPPROTO_TP
  SOCK_PTOTO_IPPROTO_DCCP = 33,		/* Datagram Congestion Control Protocol */
#define SOCK_PTOTO_IPPROTO_DCCP		SOCK_PTOTO_IPPROTO_DCCP
  SOCK_PTOTO_IPPROTO_IPV6 = 41,		/* IPv6-in-IPv4 tunnelling		*/
#define SOCK_PTOTO_IPPROTO_IPV6		SOCK_PTOTO_IPPROTO_IPV6
  SOCK_PTOTO_IPPROTO_RSVP = 46,		/* RSVP Protocol			*/
#define SOCK_PTOTO_IPPROTO_RSVP		SOCK_PTOTO_IPPROTO_RSVP
  SOCK_PTOTO_IPPROTO_GRE = 47,		/* Cisco GRE tunnels (rfc 1701,1702)	*/
#define SOCK_PTOTO_IPPROTO_GRE		SOCK_PTOTO_IPPROTO_GRE
  SOCK_PTOTO_IPPROTO_ESP = 50,		/* Encapsulation Security Payload protocol */
#define SOCK_PTOTO_IPPROTO_ESP		SOCK_PTOTO_IPPROTO_ESP
  SOCK_PTOTO_IPPROTO_AH = 51,		/* Authentication Header protocol	*/
#define SOCK_PTOTO_IPPROTO_AH		SOCK_PTOTO_IPPROTO_AH
  SOCK_PTOTO_IPPROTO_MTP = 92,		/* Multicast Transport Protocol		*/
#define SOCK_PTOTO_IPPROTO_MTP		SOCK_PTOTO_IPPROTO_MTP
  SOCK_PTOTO_IPPROTO_BEETPH = 94,		/* IP option pseudo header for BEET	*/
#define SOCK_PTOTO_IPPROTO_BEETPH		SOCK_PTOTO_IPPROTO_BEETPH
  SOCK_PTOTO_IPPROTO_ENCAP = 98,		/* Encapsulation Header			*/
#define SOCK_PTOTO_IPPROTO_ENCAP		SOCK_PTOTO_IPPROTO_ENCAP
  SOCK_PTOTO_IPPROTO_PIM = 103,		/* Protocol Independent Multicast	*/
#define SOCK_PTOTO_IPPROTO_PIM		SOCK_PTOTO_IPPROTO_PIM
  SOCK_PTOTO_IPPROTO_COMP = 108,		/* Compression Header Protocol		*/
#define SOCK_PTOTO_IPPROTO_COMP		SOCK_PTOTO_IPPROTO_COMP
  SOCK_PTOTO_IPPROTO_SCTP = 132,		/* Stream Control Transport Protocol	*/
#define SOCK_PTOTO_IPPROTO_SCTP		SOCK_PTOTO_IPPROTO_SCTP
  SOCK_PTOTO_IPPROTO_UDPLITE = 136,	/* UDP-Lite (RFC 3828)			*/
#define SOCK_PTOTO_IPPROTO_UDPLITE		SOCK_PTOTO_IPPROTO_UDPLITE
  SOCK_PTOTO_IPPROTO_RAW = 255,		/* Raw IP packets			*/
#define SOCK_PTOTO_IPPROTO_RAW		SOCK_PTOTO_IPPROTO_RAW
  SOCK_PTOTO_IPPROTO_MAX
};

struct proto {
	int (*send)(struct sock, char *buff, int length);	
	int (*bind)(struct sock, uint32_t, uint16_t);
};

struct sock* create_sock(__rte_unused int family, int proto, int mode, struct rte_ring *pipe);
void destroy_sock(struct sock* sk);


#ifdef __clpusplus
}
#endif

#endif
