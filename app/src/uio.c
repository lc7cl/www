#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "uio.h"

#define UIO_DEV "/dev/uio0"
#define UIO_MAP_SIZE "/sys/devices/platform/kcns_update_uio/uio/uio0/maps/map0/size"
#define UIO_MAP_OFFSET "/sys/devices/platform/kcns_update_uio/uio/uio0/maps/map0/offset"
#define UIO_MAP_ADDR "/sys/devices/platform/kcns_update_uio/uio/uio0/maps/map0/addr"

static struct uio_config
{
    unsigned long addr;
    int off;
    int size;
} config;
       
char* map_addr;
int uio_fd;

int open_uio()
{
    int fd, ret;

    fd = open(UIO_MAP_ADDR, O_RDONLY);
    if (fd > -1)
    {
        ret = read(fd, &config.addr, sizeof(config.addr));
        if (ret != sizeof(config.addr))
        {
            ret = -1;
            return ret;
        }
        close(fd);
    }
    else
    {
        ret = -1;
        return ret;
    }

    fd = open(UIO_MAP_OFFSET, O_RDONLY);
    if (fd > -1)
    {
        ret = read(fd, &config.off, sizeof(config.off));
        if (ret != sizeof(config.off))
        {
            ret = -1;
            return ret;
        }
        close(fd);
    }
    else
    {
        ret = -1;
        return ret;
    }

    fd = open(UIO_MAP_SIZE, O_RDONLY);
    if (fd > -1)
    {
        ret = read(fd, &config.size, sizeof(config.size));
        if (ret != sizeof(config.size))
        {
            ret = -1;
            return ret;
        }
        close(fd);
    }
    else
    {
        ret = -1;
        return ret;
    }

    fd = open(UIO_DEV, O_RDWR);
    if (fd > -1)
    {
        map_addr = (char*) mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        uio_fd = fd;
    }
    else
    {
        return -1;
    }

    return 0;
}

void close_uio()
{
    if (map_addr)
        munmap(map_addr, 4096);
    if (uio_fd > -1)
        close(uio_fd);
}

int write_uio(int flag, char *data, size_t len)
{
    struct download_head *dhead;

    if (map_addr == NULL)
    {
        printf("mmap address error!\n");
        return 1;
    }

    if (data == NULL || len == 0)
        return 1;

    dhead = (struct download_head *)(data - sizeof(struct download_head));
    if (flag == FLAG_ADD)
        dhead->flag = DOWNLOAD_ADD;
    else if (flag == FLAG_DEL)
        dhead->flag = DOWNLOAD_DEL;
    else if (flag == FLAG_SEARCH)
        dhead->flag = DOWNLOAD_SEARCH;
    else
        return 1;

    dhead->length = len + sizeof(struct download_head);
    memcpy(map_addr, (char *)dhead , dhead->length);
    return 0;
}
