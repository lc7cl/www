#ifndef _PACKET_H_
#define _PACKET_H_

#ifdef __cplusplus 
extern "C" {
#endif

#include <netinet/in.h>
#include "dns.h"

typedef struct dissect_ctx {

    FILE *dump_file;
    const struct pcap_pkthdr* pkthdr;
    const char* packet;

    const char* data;
    u_int16_t length;

    u_int16_t l2type;
    u_int16_t l3type;
    u_int16_t l4type;

    struct {
        struct in_addr sip;
        struct in_addr dip;
        u_int16_t sport;
        u_int16_t dport;

        u_int16_t id;
        int qr;
        int rcode;
        char qname[NAME_MAX_LENGTH + 32];
        int qtype;
        int qklass;

        struct {
            int mask1;
            int mask2;
            struct in_addr addr;
        } ecs;

    } packet_info;

} dissect_ctx_t;

void dispatch(u_char * arg, const struct pcap_pkthdr * pkthdr, const u_char * packet);

#ifdef __plusplus
}
#endif

#endif
