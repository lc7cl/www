#include <stdio.h>
#include <stdlib.h>

#include "command.h"

static command_t *commands;

static int get_value(option_ctx_t *ctx, option_t *options, int opt_type)
{
    switch (opt_type)
    {
    case OPTION_FILENAME:
        *ctx->value = strdup(ctx->argv);
        break;
    case OPTION_BOOL:
        *ctx->value = opt->defval;
        break;
    case OPTION_INTEGER:
        *ctx->value = atoi(ctx->argv);
        break;
    default:
        return -1;
    }
    return 0;
}

int parse_option_short(option_ctx_t *ctx, option_t *options)
{
    option_t *opt;

    opt = options;
    for (opt = options; opt->type != OPTION_MAX; opt++)
    {
        if (*ctx->opt == opt->short_name)
        {
            ctx->opt = ctx->opt[1] ? ctx->opt + 1 : NULL;
            return get_value(ctx, opt, opt->type);
        }
    }
    return -1;
}

int option_match(char *opt, char* long_name)
{
}

int parse_long_opt(option_ctx_t *ctx, option_t *options)
{
    option_t *opt;

    for (opt = options; opt->type != OPTION_END; opt++)
    {
        if (!opt->long_name)
            continue;
        if (strcmp(ctx->opt, opt->long_name))
            continue;

        return get_value(ctx, opt, opt->type);
    }
    return -1;
}

int parse_options(int argc, const char ** argv, option_t *options)
{
    option_ctx_t ctx;

    ctx->opt = NULL;
    ctx->argc = argc - 1;
    ctx->argv = argv + 1;
    ctx->out = argv;

    for (; ctx->argc; ctx->argc--, ctx->argv++)
    {
        const char* arg = ctx->argv[0];

        if (arg[0] != '-' || !arg[1])
        {
            parse_nodash_opt(ctx, options);
            continue;
        }

        if (arg[1] != '-')
        {
            ctx->opt = arg + 1;
            parse_short_opt(ctx, options);
            continue;
        }

        if (!arg[2])
        {
            parse_long_opt(ctx, options);
        }
    }

    return 0;
}

int command_register(command_t *cmd)
{
    if (cmd == NULL)
        return 1;

    if (commands == NULL)
    {
        commands = cmd;
        cmd->prev = cmd->next = NULL;
    }
    else
    {
        cmd->next = commands;
        cmd->prev = NULL;
        commands = cmd;
    }
    return 0;
}

int command_unregister(command_t *cmd)
{
    if (cmd == NULL)
        return 1;
    if (cmd == commands)
    {
        commands = cmd->next;
        commands->prev = NULL;
        cmd->next = NULL;
    }
    else
    {
        cmd->prev->next = cmd->next;
        if (cmd->next)
            cmd->next->prev = cmd->prev;
    }

    return 0;
}

command_t * get_command(const char *name)
{
    command_t *cmd;

    cmd = commands;
    while (cmd)
    {
        if (strcmp(cmd->name, name))
        {
            cmd = cmd->next;
            continue;
        }
        else
        {
            return cmd;
        }
    }

    return NULL;
}

void command_help(command_t* cmd)
{

}
