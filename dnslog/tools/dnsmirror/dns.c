#include <string.h>

#include "dns.h"

static int dns_name_from_wire(buffer_type* data, char* text_name)
{
    char *q;
    u_int8_t *p, label_len;
    int length;

    if (data == NULL || text_name == NULL)
        return -1;

    p = buffer_current(data);
    q = text_name;

    length = 0;
    label_len = buffer_read_u8(data);
    if (label_len == 0) {
        *q = '.';
        q++;
    }

    while (label_len != 0 
            && length < NAME_MAX_LENGTH
            && buffer_available(data, label_len)) {
        length += label_len;
        p = buffer_current(data);
        memcpy(q, p, label_len);
        buffer_skip(data, label_len);
        q += label_len;
        *q = '.';
        q++;
        label_len = buffer_read_u8(data);
    }

    *q = '\0';

    return 0;
}

int dns_get_qname(struct dnshdr* hdr, buffer_type* data, char* out)
{
    int ret;

    if (ntohs(hdr->qdcount) != 1)
        return -1;
    ret = dns_name_from_wire(data, out);
    if (ret == -1)
        return -1;
    return ret;
}

u_int16_t dns_get_qtype(struct dnshdr* hdr, buffer_type* buffer)
{ 
    u_int16_t qtype;

    if (ntohs(hdr->qdcount) != 1 || buffer == NULL)
        return TYPE_NONE;

    qtype = buffer_read_u16(buffer);
    if (qtype < TYPE_ALL && qtype > TYPE_NONE) {
        return qtype;
    }
    return TYPE_NONE;
}

u_int16_t dns_get_qklass(struct dnshdr* hdr, buffer_type* buffer)
{ 
    u_int16_t qklass;

    if (ntohs(hdr->qdcount) != 1 || buffer == NULL)
        return CLASS_NONE;

    qklass = buffer_read_u16(buffer);
    if (qklass < CLASS_ALL && qklass > CLASS_NONE) {
        return qklass;    
    }
    return CLASS_NONE;
}

int skip_record(buffer_type *buffer)
{
    char* p;
    u_int16_t rdlength;

    p = buffer_current(buffer);
    while (*p != '\0') {
        if (buffer_available(buffer, *p + 1))
            buffer_skip(buffer, *p + 1);
        else
            return -1;
        p = buffer_current(buffer);
    }
    if (*p != '\0')
        return -1;
    if (!buffer_available(buffer, 11))
        return -1;
    buffer_skip(buffer, 1);
    buffer_skip(buffer, 8);
    rdlength = buffer_read_u16(buffer);
    if (buffer_available(buffer, rdlength))
        return 0;
    return -1;
}

