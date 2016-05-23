#include <string.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <pcap.h>

#include "config.h"
#include "log.h"
#include "dns.h"
#include "packet.h"

static int dissect_ethernet(dissect_ctx_t* ctx)
{
    struct ether_header *eptr; 

    eptr = (struct ether_header *) ctx->data;
    ctx->l2type = ntohs(eptr->ether_type);

    ctx->data = ctx->data + sizeof(struct ether_header);
    ctx->length = ctx->length - sizeof(struct ether_header);
    return 0;
}

static int dissect_ipv4 (dissect_ctx_t* ctx)
{
    const struct ip* iphdr;
    u_int length = ctx->length;
    u_int hlen, off, version; 
    int i; 

    int len; 

    iphdr = (struct ip*)ctx->data;
    if (length < sizeof(struct ip))
    {
          log_msg("truncated ip %d\n",length);
          return -1;
    }

    len = ntohs(iphdr->ip_len);
    hlen = iphdr->ip_hl; 
    version = iphdr->ip_v;

    if(version != 4)
    {
        log_msg("Unknown version %d\n",version);
        return -1;
    }

    if(hlen < 5 )
    {
        log_msg("bad-hlen %d \n",hlen);
    }

    if(length < len)
        log_msg("\ntruncated IP - %d bytes missing\n",len - length); 

    off = ntohs(iphdr->ip_off);
    if((off & 0x1fff) == 0 )
    {
        ctx->l3type = iphdr->ip_p;
        ctx->packet_info.sip = iphdr->ip_src;
        ctx->packet_info.dip = iphdr->ip_dst;
        ctx->data = ctx->data + hlen * 4;
        ctx->length = ctx->length - hlen * 4;
        return 0;
    }
    log_msg("not support frag\n");
    return -1; 
}

static int dissect_udp(dissect_ctx_t* ctx)
{
    struct udphdr *udphdr;

    udphdr = (struct udphdr*)ctx->data;
    if (ntohs(udphdr->len) > ctx->length)
        return -1;
    ctx->packet_info.sport = ntohs(udphdr->source);
    ctx->packet_info.dport = ntohs(udphdr->dest);
    ctx->data = ctx->data + sizeof(struct udphdr);
    ctx->length = ctx->length - sizeof(struct udphdr);
    return 0;
}

static int dissect_dns(dissect_ctx_t* ctx)
{
    struct dnshdr *dnshdr;
    char *data, *p;
    int ret, i;
    buffer_type buffer;
    size_t position;

    dnshdr = (struct dnshdr*)ctx->data;
    data = (char *)(dnshdr + 1);

    ctx->packet_info.id = ntohs(ID(dnshdr));
    ctx->packet_info.qr = QR(dnshdr);
    ctx->packet_info.rcode = RCODE(dnshdr);

    buffer_create_from(&buffer, data, ctx->length - sizeof(struct dnshdr));

    ret = dns_get_qname(dnshdr, &buffer, ctx->packet_info.qname);
    if (ret == -1)
        return -1;
    ret = dns_get_qtype(dnshdr, &buffer);
    if (ret == -1)
        return -1;
    ctx->packet_info.qtype = ret;
    ret = dns_get_qklass(dnshdr, &buffer);
    if (ret == -1)
        return -1;
    ctx->packet_info.qklass = ret;

    /*skip answer section*/
    if (ctx->packet_info.qr && ntohs(dnshdr->ancount) > 64) {
        log_msg("too many answers... ");
    }
    for (i = 0; i < ntohs(dnshdr->ancount); i++) {
        if (!ctx->packet_info.qr) {
            skip_record(&buffer);
            continue;
        }        
        if (get_record(&buffer, &ctx->packet_info.answers[i]) == 0) 
            ctx->packet_info.answers_nb++;        
    }
    /*skip authority section*/
    for (i = 0; i < ntohs(dnshdr->nscount); i++) {
        skip_record(&buffer);          
    }

    for (i = 0; i < ntohs(dnshdr->arcount); i++) {
        position = buffer_position(&buffer);
        u_int8_t opt_owner;
        u_int16_t opt_type;
        u_int16_t opt_class;
        u_int32_t opt_ttl;
        u_int8_t  opt_version;
        u_int16_t opt_flags;
        u_int16_t opt_rdlen;
        u_int16_t opt_code;
        u_int16_t opt_len;

        if (!buffer_available(&buffer, 11))
            return -1;
        opt_owner = buffer_read_u8(&buffer);
        opt_type = buffer_read_u16(&buffer);
        if (opt_owner != 0 || opt_type != TYPE_OPT) {
            buffer_set_position(&buffer, position);
            skip_record(&buffer);
            continue;
        }

        opt_class = buffer_read_u16(&buffer);
        opt_ttl = buffer_read_u32(&buffer);  
        opt_version = (opt_ttl & 0x00FF0000) >> 16;
        opt_flags = (u_int16_t)(opt_ttl & 0xFFFF); 
        opt_rdlen = buffer_read_u16(&buffer);

        while (buffer_remaining(&buffer) >= 4) {
            opt_code = buffer_read_u16(&buffer);
            opt_len = buffer_read_u16(&buffer);
            if (!buffer_available(&buffer, opt_len))
                return -1;
            if (opt_code == DNS_OPT_CLIENT_SUBNET) {
                u_int16_t family;
                u_int8_t addrlen, scope, addrbytes;
                u_int32_t addr;
                int i;

                family = buffer_read_u16(&buffer);
                addrlen = buffer_read_u8(&buffer);
                scope = buffer_read_u8(&buffer);
                if (scope != 0U)
                    return -1;
                if (family != 1 || addrlen > 32U)
                    return -1;
                addrbytes = (addrlen + 7) / 8;
                if (buffer_remaining(&buffer) < addrbytes)
                    return -1;
                addr = 0;
                for (i = 0; i < addrbytes; i++) {
                    addr = addr + (buffer_read_u8(&buffer) << (8 * i));
                }
                if (addrbytes != 0U && (addrlen % 8) != 0) {
                    u_int8_t bits = ~0 << (8 - (addrlen % 8));
                    bits &= (u_int8_t)addr;
                    if (bits != (u_int8_t)addr)
                        return -1;
                }
                ctx->packet_info.ecs.addr.s_addr  = addr;
                ctx->packet_info.ecs.mask1 = addrlen;
                ctx->packet_info.ecs.mask2 = scope;
                    
            } else {
                buffer_skip(&buffer, opt_len);
            }
        }
    }
    return 0;
}

static int dissect_ctx_init(dissect_ctx_t* ctx, const struct pcap_pkthdr* pkthdr, const u_char* packet)
{
    memset(ctx, 0, sizeof(dissect_ctx_t));
    ctx->pkthdr = pkthdr;
    ctx->packet = packet;
    ctx->data = (u_char*) packet;
    ctx->length = pkthdr->len;

    return 0;
}

void dissect_ctx_cleanup(dissect_ctx_t *ctx)
{
    ctx->pkthdr = NULL;
    ctx->packet = NULL;
    memset(&ctx->packet_info, 0, sizeof(ctx->packet_info));
}

void dissect_ctx_dumpf(FILE *file, dissect_ctx_t *ctx)
{
    char buf[1024];
    int size;
    struct timeval now;
    unsigned long long timestamp;

    if (file == NULL || ctx == NULL)
        return;

    gettimeofday(&now, NULL);
    timestamp = (unsigned long long)(now.tv_sec) * 1000000 
        +  (unsigned long long)(now.tv_usec);

    fprintf(file, "%llu ", timestamp);
    size = 0;
    if (ctx->packet_info.qr == 0) {
        fprintf(file, "req:");
    } else {
        fprintf(file, "resp:");
    }
    fprintf(file, "%s|", inet_ntoa(ctx->packet_info.sip));
    fprintf(file, "%u|", ctx->packet_info.sport);
    fprintf(file, "%s|", inet_ntoa(ctx->packet_info.dip));
    fprintf(file, "%u|", ctx->packet_info.dport);
    fprintf(file, "%u|", ctx->packet_info.id);
    fprintf(file, "%s|", ctx->packet_info.qname);
    fprintf(file, "%u|", ctx->packet_info.qtype);
    fprintf(file, "%u|", ctx->packet_info.qklass);
    if (ctx->packet_info.ecs.mask1 && ctx->packet_info.qr == 0) {
        fprintf(file, "ECS%s|", inet_ntoa(ctx->packet_info.ecs.addr));
    }
    if (ctx->packet_info.qr) {
        fprintf(file, "[");
        int i;
        struct rr *record;
        for (i = 0; i < ctx->packet_info.answers_nb; i++) {
            record = &ctx->packet_info.answers[i];
            fprintf(file, "(%s,", record->name);
            switch (record->type) {
            case TYPE_A:
                fprintf(file, "%s,", "A");
                fprintf(file, NIPQUAD_FMT")", (uint32_t*)record->rdata);
                break;
            case TYPE_CNAME:
                fprintf(file, "%s,", "CNAME");
                fprintf(file, "%s)", record->rdata);
                break;
            default:
                fprintf(file, "%s,", "N/A");
                fprintf(file, "%s)", "N/A");
                break;
            }
            if (i != ctx->packet_info.answers_nb - 1)
                fprintf(file, ",");
        }
        fprintf(file, "]|");
    }
    fprintf(file, "%d\n", ctx->packet_info.rcode);
    fflush(file);
}

void dissect_packet(u_char * arg, const struct pcap_pkthdr * pkthdr, const u_char * packet, FILE* out)
{
    dissect_ctx_t* ctx = (dissect_ctx_t*)arg;

    if (ctx == NULL || pkthdr == NULL || packet == NULL)
        return;

    if (dissect_ctx_init(ctx, pkthdr, packet))
        return;

    if (dissect_ethernet(ctx) == -1 || ctx->l2type != ETHERTYPE_IP) {
        //log_msg("not ip?\n");
        goto error;
    }

    if (dissect_ipv4(ctx) == -1 || ctx->l3type != IPPROTO_UDP)
        //log_msg("not udp?\n");
        goto error;

    if (dissect_udp(ctx) == -1 || ctx->packet_info.dport != 53) {
        log_msg("not 53 port? %u\n", ctx->packet_info.dport);
        goto error;
    }

    if (dissect_dns(ctx) == -1) {
        log_msg("dns ?\n");
        goto error;
    }

    dissect_ctx_dumpf(out, ctx);
out:
    dissect_ctx_cleanup(ctx);
    return;
error:
    goto out;
}


