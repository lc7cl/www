#ifndef _EDNS_H
#define _EDNS_H

#include <stdio.h>

#define ACTION_ADD 1
#define ACTION_DEL 2
#define ACTION_SEARCH 3

typedef struct edns_context
{
	int action;
	char *ip;
	FILE *input;
	FILE *output;
} edns_context_t;

#define INIT_EDNS_CTX(ctx) do { memset(&ctx, 0, sizeof(edns_context_t)); } while (0)
#define DESTROY_EDNS_CTX(ctx) do {\
		if (ctx.ip)                   \
			free(ctx.ip);             \
		if (ctx.input)                \
			fclose(ctx.input);        \
		if (ctx.output)				  \
		{							  \
			fflush(ctx.output);       \
			fclose(ctx.output);		  \
		}                             \
	} while (0)
extern int edns_init();
extern void edns_close();
extern int edns_setting(edns_context_t *ctx);
int edns_command_register();
int edns_command_unregister();

#endif
