/*
*                           _ooOoo_
*                          o8888888o
*                          88" . "88
*                          (| ^_^ |)
*                          O\  =  /O
*                       ____/`---'\____
*                     .'  \\|     |//  `.
*                    /  \\|||  :  |||//  \
*                   /  _||||| -:- |||||-  \
*                   |   | \\\  -  /// |   |
*                   | \_|  ''\---/''  |   |
*                   \  .-\__  `-`  ___/-. /
*                 ___`. .'  /--.--  `. . ___ 
*               ."" '<  `.___\_<|>_/___.'  >'"".
*             | | :  `- \`.;`\ _ /`;.`/ - ` : | |
*             \  \ `-.   \_ __\ /__ _/   .-` /  /
*       ========`-.____`-.___\_____/___.-`____.-'======== 
*                            `=---='
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*            FoZu Bless       No Bug       No Crash
*
* Author: lee - lee@www.com
*
* Last modified: 2015-02-24 00:04
*
* Filename: command.c
*
* Description: 
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"

static command_t *commands;

static int option_match(char *opt, char* long_name)
{
    return 0;
}

static int get_value(option_ctx_t *ctx, option_t *option, int opt_type)
{
    switch (opt_type)
    {
    case OPTION_FILENAME:
        option->value = strdup(ctx->argv[0]);
        break;
    case OPTION_BOOL:
        *(int*)option->value = option->defval;
        break;
    case OPTION_INTEGER:
        *(int*)option->value = atoi(ctx->argv[0]);
        break;
    default:
        return -1;
    }
    return 0;
}

static int parse_nodash_opt(option_ctx_t *ctx, option_t *options)
{
    return 0;
}

static int parse_short_opt(option_ctx_t *ctx, option_t *options)
{
    option_t *opt;

    opt = options;
    for (opt = options; opt->type != OPTION_END; opt++)
    {
    }
    return -1;
}

static int parse_long_opt(option_ctx_t *ctx, option_t *options)
{
    option_t *opt;

    for (opt = options; opt->type != OPTION_END; opt++)
    {
        if (!opt->long_name)
            continue;

        return get_value(ctx, opt, opt->type);
    }
    return -1;
}

int parse_options(int argc, const char ** argv, option_t *options)
{
    option_ctx_t ctx;

    ctx.argc = argc - 1;
    ctx.argv = argv + 1;
    ctx.out = argv;

    for (; ctx.argc; ctx.argc--, ctx.argv++)
    {
        const char* arg = ctx.argv[0];

        if (arg[0] != '-' || !arg[1])
        {
            parse_nodash_opt(&ctx, options);
            continue;
        }

        if (arg[1] != '-')
        {
            parse_short_opt(&ctx, options);
            continue;
        }

        if (!arg[2])
        {
            parse_long_opt(&ctx, options);
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
