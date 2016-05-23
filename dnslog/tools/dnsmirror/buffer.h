#ifndef _BUFFER_H_
#define _BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stddef.h"
#include "string.h"
#include "sys/types.h"

typedef struct buffer buffer_type;

struct buffer
{
    /*
     *   * The current position used for reading/writing.
     *       */ 
    size_t   _position;

    /*
     *   * The read/write limit.
     *       */
    size_t   _limit;

    /*
     *   * The amount of data the buffer can contain.
     *       */
    size_t   _capacity;

    /*
     *   * The data contained in the buffer.
     *       */
    u_int8_t *_data;

    /*
     *   * If the buffer is fixed it cannot be resized.
     *       */
    unsigned _fixed : 1;
};


buffer_type *buffer_create(size_t capacity);
void buffer_create_from(buffer_type *buffer, void *data, size_t size);
void buffer_clear(buffer_type *buffer);

static inline size_t
buffer_position(buffer_type *buffer)
{
    return buffer->_position;
}

static inline void
buffer_set_position(buffer_type *buffer, size_t mark)
{
    buffer->_position = mark;
}

static inline void
buffer_skip(buffer_type *buffer, ssize_t count)
{
    buffer->_position += count;
}

static inline size_t
buffer_limit(buffer_type *buffer)
{
    return buffer->_limit;
}

static inline void
buffer_set_limit(buffer_type *buffer, size_t limit)
{
    buffer->_limit = limit;
    if (buffer->_position > buffer->_limit)
        buffer->_position = buffer->_limit;
}

static inline size_t
buffer_capacity(buffer_type *buffer)
{
    return buffer->_capacity;
}

static inline u_int8_t *
buffer_at(buffer_type *buffer, size_t at)
{
    return buffer->_data + at;
}

static inline u_int8_t *
buffer_current(buffer_type *buffer)
{
    return buffer_at(buffer, buffer->_position);
}

static inline size_t
buffer_remaining_at(buffer_type *buffer, size_t at)
{
    return buffer->_limit - at;
}

static inline size_t
buffer_remaining(buffer_type *buffer)
{
    return buffer_remaining_at(buffer, buffer->_position);
}

static inline int
buffer_available_at(buffer_type *buffer, size_t at, size_t count)
{
    return count <= buffer_remaining_at(buffer, at);
}

static inline int
buffer_available(buffer_type *buffer, size_t count)
{
    return buffer_available_at(buffer, buffer->_position, count);
}

static inline u_int16_t
read_uint16(const void *src)
{
#ifdef ALLOW_UNALIGNED_ACCESSES
    return ntohs(* (u_int16_t *) src);
#else
    const u_int8_t *p = (const u_int8_t *) src;
    return (p[0] << 8) | p[1];
#endif
}

static inline u_int32_t
read_uint32(const void *src)
{
#ifdef ALLOW_UNALIGNED_ACCESSES
    return ntohl(* (u_int32_t *) src);
#else
    const u_int8_t *p = (const u_int8_t *) src;
    return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
#endif
}

static inline void
buffer_read_at(buffer_type *buffer, size_t at, void *data, size_t count)
{
	memcpy(data, buffer->_data + at, count);
}

static inline void
buffer_read(buffer_type *buffer, void *data, size_t count)
{
	buffer_read_at(buffer, buffer->_position, data, count);
	buffer->_position += count;
}

static inline u_int8_t
buffer_read_u8_at(buffer_type *buffer, size_t at)
{
    return buffer->_data[at];
}

static inline u_int8_t
buffer_read_u8(buffer_type *buffer)
{
    u_int8_t result = buffer_read_u8_at(buffer, buffer->_position);
    buffer->_position += sizeof(u_int8_t);
    return result;
}

static inline u_int16_t
buffer_read_u16_at(buffer_type *buffer, size_t at)
{
    return read_uint16(buffer->_data + at);
}

static inline u_int16_t
buffer_read_u16(buffer_type *buffer)
{
    u_int16_t result = buffer_read_u16_at(buffer, buffer->_position);
    buffer->_position += sizeof(u_int16_t);
    return result;
}

static inline u_int32_t
buffer_read_u32_at(buffer_type *buffer, size_t at)
{
    return read_uint32(buffer->_data + at);
}

static inline u_int32_t
buffer_read_u32(buffer_type *buffer)
{
    u_int32_t result = buffer_read_u32_at(buffer, buffer->_position);
    buffer->_position += sizeof(u_int32_t);
    return result;
}

#ifdef __cpluscplus
}
#endif

#endif
