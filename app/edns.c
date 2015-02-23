#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include "command.h"
#include "uio.h"
#include "edns.h"

#define EDNS_PROC "/proc/sys/net/ipv4/kcns_server/edns"

static int edns_fd;

static int ip_str_to_int(char *ip, int len, unsigned int *val)
{
    int ret;
    unsigned int v = 0;
    char c;
    int dot_count = 0, d = 0;
    char *p, *e;

    p = ip;
    e = ip + len;
    while (*p && (p != e))
    {
        c = *p;
        if ((c >= '0') && (c <= '9'))
        {
            d = d * 10 + c - '0';
            if (d > 255)
               return 1;
        } 
        else if (c == '.')
        {
            v += d << ( 8);
            d = 0;
            dot_count++;
            if (dot_count > 3 )
                return 1;
        }
        p++;
    }
    if (p == e)
    {
        v += d << ( 8);
		if (d > 255)
               return 1;
    }

    *val = v;
    return 0;
}

static int open_edns_proc()
{
    int fd;

    fd = open(EDNS_PROC, O_RDWR);

    return fd;
}

static int write_edns_proc(int fd, int i)
{
    int ret = 0, size = 0;
    char buff[22];
    if (fd)
    {
        size = snprintf(buff, 21, "%d", i);
        size = write(fd, buff, size);
        if (size > 0)
            ret = 0;
        else {
            printf("error:%d %s \n", errno, strerror(errno));
        }
    }
    else
        ret = 1;

    return ret;
}

static int read_edns_proc(int fd)
{
    int ret = -1, size;
    char buff[100];
    memset(buff, 0, sizeof(buff));
    if(fd)
    {
        if (size = read(fd, buff, sizeof(buff)))
        {
            ret = atoi(buff);
        }
        else
        {
            printf("%s %d error:%d %s  size:%d\n", __func__, __LINE__, errno, strerror(errno), size);
        }
    }
    return ret;
}

static void close_edns_proc(int fd)
{
    if (fd > -1)
        close(fd);
}

int edns_init()
{
    edns_fd = open_edns_proc();
    if (edns_fd > -1)
        return 0;
    else
    	return 1;
}

void edns_close()
{
    close_edns_proc(edns_fd);
}

int edns_setting(int action, char *ip, int len)
{
    int flag;
    unsigned int i;
    char *buf;
    int val;

    if (action == ACTION_ADD)
        flag = FLAG_ADD;
    else if (action == ACTION_DEL)
        flag = FLAG_DEL;
    else if (action == ACTION_SEARCH)
        flag = FLAG_SEARCH;
    else
        return 1;

    if (!ip_str_to_int(ip, len, &i))
    {
        buf = malloc(sizeof(struct download_head) + sizeof(i));
        buf = buf + sizeof(struct download_head);
        memcpy(buf, &i, sizeof(i));
        write_uio(flag, buf, sizeof(i));
        return write_edns_proc(edns_fd, 1);
    }
    else
    {
        printf("IP format invalid!\n");
    }
    return 1;
}

static int command_edns(int argc, const char** argv)
{

    int flag, i;
    char *filename;
    const char *pargv;
    char *pc;

    option_t cmd_opts[] = {
        MK_OPTION_BOOL('a', "add", &flag, "add edns ip"),
        MK_OPTION_BOOL('d', "delete", &flag, "delete edns ip"),
        MK_OPTION_BOOL('s', "search", &flag, "search edns ip"),
        MK_OPTION_FILENAME('f', "file", &filename, "add/delete edns ip in file"),    
        MK_OPTION_MAX(),
    };

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
