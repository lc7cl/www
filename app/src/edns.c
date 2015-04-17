#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>

#include "common.h"
#include "command.h"
#include "uio.h"
#include "edns.h"

int edns_setting(edns_context_t *ctx)
{
    int flag, ret, res = 0;;
    unsigned int i;
    char *buf, *pch, *p;
    int val;

	if (!ctx)
		return -1;

    if (ctx->action == ACTION_ADD)
        flag = FLAG_ADD;
    else if (ctx->action == ACTION_DEL)
        flag = FLAG_DEL;
    else if (ctx->action == ACTION_SEARCH)
        flag = FLAG_SEARCH;
    else
        return 1;
    
    buf = malloc(sizeof(struct download_head) + 128);

	if (ctx->ip)
	{
	    pch = strtok(ctx->ip, "  \n");
	    while (pch != NULL)
	    {
	        printf("%s\n", pch);
	        i = inet_network(pch);
	        if (i != -1)
	        {
	            memset(buf, 0, sizeof(struct download_head) + 128);
	            p = buf + sizeof(struct download_head);
	            memcpy(p, &i, sizeof(i));
#if 1
	            ret = write_uio(flag, p, sizeof(i));
	            if (ret)
	            {
	                if (buf)
	                    free(buf);
	                return ret;
	            }
				if (ctx->action == ACTION_SEARCH && ctx->output)
				{
					read_uio((char *)&res, sizeof(res));
					fprintf(ctx->output, "IP:%s RESULT:%d\n", pch, res);
				}
#endif
	        }
	        else
	        {
	            printf("invalid ip %s\n", pch);
	        }
	        pch = strtok(NULL, "  \n");
	    }
	}

	if (ctx->input)
	{
	#define LINESIZE 1024
		char *p;
		char line_buf[LINESIZE];

		memset(line_buf, 0, LINESIZE);
		while (fgets(line_buf, LINESIZE, ctx->input))
		{
			pch = strtok(line_buf, "  \n");
		    while (pch != NULL)
		    {
		        printf("%s\n", pch);
		        i = inet_network(pch);
		        if (i != -1)
		        {
		            memset(buf, 0, sizeof(struct download_head) + 128);
		            p = buf + sizeof(struct download_head);
		            memcpy(p, &i, sizeof(i));
		            ret = write_uio(flag, p, sizeof(i));
		            if (ret)
		            {
		                if (buf)
		                    free(buf);
		                return ret;
		            }
					if (ctx->action == ACTION_SEARCH && ctx->output)
					{
						read_uio((char *)&res, sizeof(res));
						fprintf(ctx->output, "IP:%s RESULT:%d\n", pch, atoi((char*)&res));
					}
		        }
		        else
		        {
		            printf("invalid ip %s\n", pch);
		        }
		        pch = strtok(NULL, "  \n");
		    }
		}
	}
	
    return 0;
}

static int command_edns(int argc, const char** argv)
{

    int flag, i;
    char *filename;
    const char *pargv;
    const char *pc;

    option_t cmd_opts[] = {
        MK_OPTION_BOOL('a', "add", &flag, "add edns ip"),
        MK_OPTION_BOOL('d', "delete", &flag, "delete edns ip"),
        MK_OPTION_BOOL('s', "search", &flag, "search edns ip"),
        MK_OPTION_FILENAME('f', "file", &filename, "add/delete edns ip in file"),    
        MK_OPTION_END(),
    };

    parse_options(argc, argv, cmd_opts);

    for (i = 1; i < argc; i++)
    {
        pargv = argv[i];

        pc = pargv;
        while (*pc != '\0')
        {
            if (*pc == '-')
            {
                /*short name*/
                if (*(pc+1) != '\0' && (*(pc+1) != '-'))
                {
                    //for 
                }
            }
        }

    }

    return 0;
}

static command_t edns_cmd = {
    .name = "edns",
    .handler = command_edns,
    .next = NULL,
    .prev = NULL,
};

int edns_command_register()
{
    return command_register(&edns_cmd);
}

int edns_command_unregister()
{
    return command_register(&edns_cmd);
}
