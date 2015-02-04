#ifndef _CMD_H
#define _CMD_H

typedef int (*cmd_handler_t)(int, const char**);

enum cmd_opt_type
{
    CMD_OPT_STRING,
    CMD_OPT_INTEGER,
    CMD_OPT_FILENAME,
    CMD_OPT_BOOL,
    CMD_OPT_END
};

typedef struct command
{
    const char *name;
    cmd_handler_t *handler;
    struct command *next, *prev;
} command_t;

typedef struct command_opt
{
    int type;
    int shortname;
    const char* longname;
    void *value;
    const char* helper;
    int defval;
} command_opt_t;

typedef opt_ctx
{
    int argc;
    const char **argv, **out;
    int type;
}

#define MK_OPT_STRING(sn, ln, val, h) {CMD_OPT_STRING, sn, ln, val, h}
#define MK_OPT_INTEGER(sn, ln, val, h) {CMD_OPT_INTEGER, sn, ln, val, h}
#define MK_OPT_FILENAME(sn, ln, val, h) {CMD_OPT_FILENAME, sn, ln, val, h}
#define MK_OPT_BOOL(sn, ln, val, h) {CMD_OPT_FILENAME, sn, ln, val, h, 1}
#define MK_OPT_END() {CMD_OPT_END}

int command_register(command_t*);
int command_unregister(command_t*);
command_t * get_command(const char*);
void command_help(commant_t*);
int parse_opt_short(opt_ctx_t*, command_opt_t);

#endif
