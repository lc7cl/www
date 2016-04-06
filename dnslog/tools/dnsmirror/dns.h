#ifndef _DNS_H_
#define _DNS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "buffer.h"

#define NAME_MAX_LENGTH 512

#define DNS_OPT_CLIENT_SUBNET   0x0008

struct dnshdr {
    u_int16_t id;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    u_int16_t rd:1,    
              tc:1,
              aa:1,
              opcode:4,    
              qr:1,    
              rcode:4, 
              cd:1,     
              ad:1,
              z:1,
              ra:1;
#elif __BYTE_ORDER == __BIG_ENDIAN
    u_int16_t qr:1,
              opcode:4,
              aa:1,
              tc:1,
              rd:1,
              ra:1,
              z:1,
              ad:1,
              cd:1,
              rcode:4;
#else
# error	"Please fix <bits/endian.h>"
#endif
    u_int16_t qdcount;
    u_int16_t ancount;
    u_int16_t nscount;
    u_int16_t arcount;
} __attribute__((__packed__));

enum dns_type {
    TYPE_NONE  = 0x00,  /* No type definition.      */
    TYPE_A     = 0x01,  /* Address type             */
    TYPE_NS    = 0x02,  /* Name Server type         */
    TYPE_CNAME = 0x05,  /* Canonical name type      */
    TYPE_SOA   = 0x06,  /* Start of Authority type  */
    TYPE_PTR   = 0x0c,  /* Pointer type             */
    TYPE_HINFO = 0x0d,  /* Host Information type    */
    TYPE_MX    = 0x0f,  /* Mail Exchanger type      */
    TYPE_TXT   = 0x10,  /* Text type                */
    TYPE_A6    = 0x1c,  /* Address (IP6) type       */
    TYPE_SRV   = 0x21,  /* Service type             */
    TYPE_OPT    = 0x29,  /* EDNS's OPT type          */              
    TYPE_AX    = 0x68,  /* AX type                  */
    TYPE_CNAMEX= 0x69,  /* CNAMEX type              */
    TYPE_LINK  = 0x70,  /* LINK type            */
    TYPE_TSIG  = 0xfa,  /* tsig type                */
    TYPE_AXFR  = 0xfc,  /* Zone transfer type       */
    TYPE_ALL   = 0xff   /* Query all types          */
};

enum dns_class {
    CLASS_NONE  = 0x00,  /* No class definition                         */
    CLASS_IN,            /* Internet class. Default class               */
    CLASS_CS,            /* CSNET class, obsolete, used for examples.   */
    CLASS_CH,            /* CHAOS class                                 */
    CLASS_HS,            /* Hesiod [Dyer 87]                            */
    CLASS_ALL  = 0xff    /* Query all classes                           */
};

enum dns_opcode {          
    SQUERY = 0,              /* Standard query                */
    IQUERY,                 /* Inverse query                 */
    STATUS                  /* Server status query           */
};


enum dns_rcode {                                                         
    NOERROR = 0,       /* 0: No error (RFC1035)                     */
    FORMERR = 1,       /* 1: Format error (RFC1035)                 */
    SERVFAIL = 2,      /* 2: Server failure (RFC1035)               */
    NXDOMAIN = 3,      /* 3: Name Error (RFC1035)                   */
    NOTIMP = 4,        /* 4: Not Implemented (RFC1035)              */
    REFUSED = 5,       /* 5: Refused (RFC1035)                      */
    YXDOMAIN = 6,      /* 6: Name unexpectedly exists (RFC2136)     */
    YXRRSET = 7,       /* 7: RRset unexpectedly exists (RFC2136)    */
    NXRRSET = 8,       /* 8: RRset should exist but not (RFC2136)   */
    NOTAUTH = 9,       /* 9: Server isn't authoritative (RFC2136)   */
    NOTZONE = 10,      /* 10: Name is not within the zone (RFC2136) */
    RESERVED11 = 11,   /* 11: Reserved for future use (RFC1035)     */
    RESERVED12 = 12,   /* 12: Reserved for future use (RFC1035)     */
    RESERVED13 = 13,   /* 13: Reserved for future use (RFC1035)     */
    RESERVED14 = 14,   /* 14: Reserved for future use (RFC1035)     */
    RESERVED15 = 15,   /* 15: Reserved for future use (RFC1035)     */
    BADVERS = 16       /* 16: EDNS version not implemented (RFC2671)*/
};

#define ID(hdr) (((struct dnshdr*)hdr)->id)
#define QR(hdr) (((struct dnshdr*)hdr)->qr)
#define RCODE(hdr) (((struct dnshdr*)hdr)->rcode)

int dns_get_qname(struct dnshdr* hdr, buffer_type* data, char* out);
u_int16_t dns_get_qtype(struct dnshdr* hdr, buffer_type* buffer);
u_int16_t dns_get_qklass(struct dnshdr* hdr, buffer_type* buffer);

#ifdef __cplusplus
}
#endif

#endif
