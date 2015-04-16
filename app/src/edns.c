#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>

#include "command.h"
#include "uio.h"
#include "edns.h"

int edns_setting(int action, char *ip, size_t len)
{
    int flag, ret;
    unsigned int i;
    char *buf, *pch, *p;
    int val;

    if (action == ACTION_ADD)
        flag = FLAG_ADD;
    else if (action == ACTION_DEL)
        flag = FLAG_DEL;
    else if (action == ACTION_SEARCH)
        flag = FLAG_SEARCH;
    else
        return 1;
    
    buf = malloc(sizeof(struct download_head) + 128);
    pch = strtok(ip, "  \n");
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
            ret = write_uio(flag, buf, sizeof(i));
            if (ret)
            {
                if (buf)
                    free(buf);
                return ret;
            }
            ret = write_edns_proc(edns_fd, 1);
            if (ret)
            {
                if (buf)
                    free(buf);
                return ret;
            }
#endif
        }
        else
        {
            printf("invalid ip %s\n", pch);
        }
        pch = strtok(NULL, "  \n");
    }
    return 1;
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
