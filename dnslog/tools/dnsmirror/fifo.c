#include <stdio.h>
#include <stdlib.h>

#include "fifo.h"

struct fifo* fifo_create(int elem_nb, size_t elem_sz)
{
    struct fifo* new;

    new = malloc(sizeof(struct fifo) + elem_sz * elem_nb);
    if (new == NULL)
        return NULL;

    new->q = (void*)(new + 1);
    new->head = 0;
    new->tail = 0;
    new->elem_sz = elem_sz;
    new->elem_nb = elem_nb;
    return new;
}

void* fifo_get(struct fifo* f)
{
    void *elem;

    if (f == NULL)
        return NULL;

    if (f->head == f->tail)
        return NULL;

    elem = f->q[f->head];
    f->head++;
    if (f->head >= f->elem_nb)
        f->head = 0;
    return elem;
}

int fifo_put(struct fifo* f, void* elem)
{
    if (f == NULL || elem == NULL) 
        return -1;
    if (((f->tail > f->head) && (f->head + f->elem_nb - f->tail > 1))
            || (f->tail < f->head) && (f->head - f->tail > 1)
            || f->head == f->tail) {
        f->q[f->tail] = elem;
        f->tail++;
        if (f->tail >= f->elem_nb)
            f->tail = 0;
        return 0;
    }
    return -1;
}

void fifo_destroy(struct fifo* f) 
{
    if (f)
        free(f);
}
