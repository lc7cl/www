#ifndef _CMD_H
#define _CMD_H

typedef void (*cmd_handler_t)(int, const char**);

struct cmd_info
{
    const char *name;
    cmd_handler_t *handler;
    cmd_info *next, *prev;
} cmd_info_t;

struct cmdopt
{
    int type;
    int shortname;
    const char* longname;
    int void *value;
} cmdopt_t;

#endif
