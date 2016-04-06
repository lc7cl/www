#include <stdlib.h>

#include "buffer.h"

buffer_type *
buffer_create(size_t capacity)
{
    buffer_type *buffer
        = (buffer_type *) malloc(sizeof(buffer_type));
    if (!buffer)
        return NULL;

    buffer->_data = (u_int8_t *)malloc(capacity);
    buffer->_position = 0;
    buffer->_limit = buffer->_capacity = capacity;
    buffer->_fixed = 0;
    
    return buffer;
}

void
buffer_create_from(buffer_type *buffer, void *data, size_t size)
{
    buffer->_position = 0;
    buffer->_limit = buffer->_capacity = size;
    buffer->_data = (u_int8_t *) data;
    buffer->_fixed = 1;
}

void
buffer_clear(buffer_type *buffer)
{
    buffer->_position = 0;
    buffer->_limit = buffer->_capacity;
}

void
buffer_flip(buffer_type *buffer)
{
    buffer->_limit = buffer->_position;
    buffer->_position = 0;
}

void
buffer_rewind(buffer_type *buffer)
{
    buffer->_position = 0;
}

void
buffer_set_capacity(buffer_type *buffer, size_t capacity)
{
    buffer->_data = (u_int8_t *)malloc(capacity);
    buffer->_limit = buffer->_capacity = capacity;
}
