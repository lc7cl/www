#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

static int nl_sock;

int netlink_open()
{
    int ret;
    sockaddr_nl addr; 

    nl_sock = socket(AF_NETLINK, SOCK_DGRAM, 0);
    if (nl_sock > 0)
        return 0;

    memset(&addr, 0, sizeof(sockaddr_nl));
    addr.nl_family = AF_NETLINK;
    addr.nl_pid = 0;
    addr.nl_groups = 0;
    

    
    return 1;
}


void netlink_close()
{
    close(nl_sock);
}


int netlink_send(const char* data, size_t length)
{
    if (!data)
        return -1;

    return 0;
}

int netlink_receive(char* buf, size_t length)
{
    if (!buf)
        return -1;

    return 0;
}
