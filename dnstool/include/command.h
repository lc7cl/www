#ifndef _COMMAND_H
#define _COMMADD_H

enum option_type
{
    OPTION_STRING = 1,
    OPTION_INTEGER,
    OPTION_FILENAME,
    OPTION_BOOL,
    OPTION_END
};

typedef struct command
{
    const char *name;
    int (*handler)(int, const char**);
    struct command *next, *prev;
} command_t;

typedef struct option
{
    int type;
    int short_name;
    const char* long_name;
    void *value;
    const char* helper;
    int defval;
} option_t;

typedef struct option_ctx
{
    int argc;
    const char **argv, **out;
    const char *opt;
    int type;
} option_ctx_t;

#define MK_OPTION_STRING(sn, ln, val, h) {OPTION_STRING, sn, ln, val, h}
#define MK_OPTION_INTEGER(sn, ln, val, h) {OPTION_INTEGER, sn, ln, val, h}
#define MK_OPTION_FILENAME(sn, ln, val, h) {OPTION_FILENAME, sn, ln, val, h}
#define MK_OPTION_BOOL(sn, ln, val, h) {OPTION_FILENAME, sn, ln, val, h, 1}
#define MK_OPTION_END() {OPTION_END}

int command_register(command_t*);
int command_unregister(command_t*);
command_t * get_command(const char*);
void command_help(command_t*);
int parse_options(int argc, const char ** argv, option_t *options);

#endif
