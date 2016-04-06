#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "task.h"

struct task* create_task(const char* name, struct fifo* pipe, task_func action)
{
    struct task* t;
    char buf[1024];

    t = malloc(sizeof(struct task));
    if (t == NULL) {
        return NULL;
    }
    t->pipe = pipe;
    t->action = action;
    t->name = strdup(name);
    if (t->name == NULL) {
        free(t);
        return NULL;
    }

    snprintf(buf, 1024, "%s/%s.log", g_config.output_path, name);
    t->output = fopen(buf, "w+");
    if (t->output == NULL) {
        free(t->name);
        free(t);
        return NULL;
    }

    t->stop = 0;
    return t;
}

void destroy_task(struct task* t)
{
    if (t) {
        if (t->name)
            free(t->name);
        free(t);
    }
}

