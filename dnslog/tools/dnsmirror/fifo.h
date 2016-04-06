#ifndef _FIFO_H_
#define _FIFO_H_

#ifdef __cplusplus
extern "C" {
#endif

#define FIFO_MAX_LENGTH 4096

struct fifo {
    int head, tail, elem_sz, elem_nb;
    void** q;    
};

struct fifo* fifo_create(int elem_nb, size_t elem_sz);
void* fifo_get(struct fifo* f);
int fifo_put(struct fifo* f, void* elem);
void fifo_destroy(struct fifo* f);

#ifdef __cplusplus
}
#endif

#endif
