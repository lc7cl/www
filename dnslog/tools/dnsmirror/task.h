#ifndef _TASK_H_
#define _TASK_H_

#ifdef __cpluscplus
extern "C" {
#endif

#include <stdio.h>

typedef void* (*task_func)(void*);
struct task {
    char* name;
    struct fifo* pipe;
    task_func action;
    FILE *output;
    int stop;
};

struct task* create_task(const char* name, struct fifo* pipe, task_func action);
void destroy_task(struct task* t);

#ifdef __cplusplus
}
#endif

#endif
