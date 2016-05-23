#include <string.h>

#include "dns.h"

static int dns_name_from_wire(buffer_type* data, 
                              struct decompress_ctx *decompress, 
                              char* text_name)
{
    char *q;
    u_int8_t *p, label_len;
    int length, off, saved = -1, i;

    if (data == NULL || text_name == NULL || decompress == NULL)
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
            && length < NAME_MAX_LENGTH) {
        if ((label_len & 0xc0) == 0xc0) {
            buffer_set_position(data, buffer_position(data) - 1);
            label_len = buffer_read_u16(data);
            off = label_len & ~0xc0;
            if (off >= buffer_limit(data) /*|| decompress->pos[off] != off*/)
                return -1;
            if (saved == -1)
                saved = buffer_position(data);
            buffer_set_position(data, off);
            label_len = buffer_read_u8(data);            
        } else {
            if (!buffer_available(data, label_len))
                return -1;
            for (i = 0; i < label_len; i++) {
                if (decompress->pos[i] == -1)
                    decompress->pos[i] = buffer_position(data);
            }
            length += label_len;
            p = buffer_current(data);
            memcpy(q, p, label_len);
            buffer_skip(data, label_len);
            q += label_len;
            *q = '.';
            q++;
            label_len = buffer_read_u8(data);
        }

    }
    
    if (label_len == 0) { 
        *q = '\0';       
        if (saved > -1)
            buffer_set_position(data, saved);
    } else {
        log_msg("dns:domain not ends with \\0\n");
        return -1;
    }  

    return 0;
}

int dns_get_qname(struct dnshdr* hdr, 
                  buffer_type* data, 
                  struct decompress_ctx *decompress, 
                  char* out)
{
    int ret;

    if (ntohs(hdr->qdcount) != 1)
        return -1;
    ret = dns_name_from_wire(data, decompress, out);
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

int get_record(buffer_type *buffer,
               struct decompress_ctx * decompress, 
               struct rr *record)
{
    int ret;
    
    if (buffer == NULL || record == NULL)
        return -1;
    ret = dns_name_from_wire(buffer, decompress, record->name);
    if (ret == -1)
        return -1;
    if (!buffer_available(buffer, 2 * sizeof(u_int16_t)))
        return -1;
    record->type = buffer_read_u16(buffer);
    buffer_skip(buffer, 
                sizeof(u_int16_t) + sizeof(u_int32_t) + sizeof(u_int16_t));
    switch (record->type) {
    case TYPE_A:
        if (!buffer_available(buffer, sizeof(u_int32_t)))
            return -1;
        buffer_read(buffer, record->rdata, 4);
        break;
    case TYPE_CNAME:
        ret = dns_name_from_wire(buffer, decompress, record->rdata);
        if (ret == -1)
            return ret;
        break;
    default:
        return -1;
    }
    return 0;
}

