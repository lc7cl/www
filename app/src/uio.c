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
#define UIO_CTRL "/proc/sys/net/ipv4/kcns_server/uio_ctrl"

#define uio_error(fmt, ...) printf("uio_error:""fmt, ...");

static struct uio_config
{
    unsigned long addr;
    int off;
    int size;
	int uio_fd;
	int ctl_fd;
	char* map_addr;
} uio_cfg;
       
int open_uio()
{
    int fd, ret;

    fd = open(UIO_MAP_ADDR, O_RDONLY);
    if (fd > -1)
    {
        ret = read(fd, &uio_cfg.addr, sizeof(uio_cfg.addr));
        if (ret != sizeof(uio_cfg.addr))
        {
            ret = -1;
            return ret;
        }
        close(fd);
    }
    else
    {    
		uio_error("uio_error:fail open %s\n", UIO_MAP_ADDR);
        ret = -1;
        return ret;
    }

    fd = open(UIO_MAP_OFFSET, O_RDONLY);
    if (fd > -1)
    {
        ret = read(fd, &uio_cfg.off, sizeof(uio_cfg.off));
        if (ret != sizeof(uio_cfg.off))
        {
            ret = -1;
            return ret;
        }
        close(fd);
    }
    else
    {    
		uio_error("uio_error:fail open %s\n", UIO_MAP_OFFSET);
        ret = -1;
        return ret;
    }

    fd = open(UIO_MAP_SIZE, O_RDONLY);
    if (fd > -1)
    {
        ret = read(fd, &uio_cfg.size, sizeof(uio_cfg.size));
        if (ret != sizeof(uio_cfg.size))
        {
            ret = -1;
            return ret;
        }
        close(fd);
    }
    else
    {    
		uio_error("uio_error:fail open %s\n", UIO_MAP_SIZE);
        ret = -1;
        return ret;
    }

    fd = open(UIO_DEV, O_RDWR);
    if (fd > -1)
    {
        uio_cfg.map_addr = (char*) mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        uio_cfg.uio_fd = fd;
    }
    else
    {    
		uio_error("uio_error:fail open %s\n", UIO_DEV);
        return -1;
    }

	fd = open(UIO_CTRL, O_RDWR);
	
    if (fd > -1)
    {
	    uio_cfg.ctl_fd = fd;    
    }
	else
	{
		uio_error("fail open %s\n", UIO_CTRL);
		return -1;
	}
    return 0;
}

void close_uio()
{
    if (uio_cfg.map_addr)
        munmap(uio_cfg.map_addr, 4096);
    if (uio_cfg.uio_fd > -1)
        close(uio_cfg.uio_fd);
	if (uio_cfg.ctl_fd > -1)
		close(uio_cfg.ctl_fd);
}

int write_uio(int flag, char *data, size_t len)
{
    struct download_head *dhead;	
    char buff[22];
	int size;

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
    memcpy(uio_cfg.map_addr, (char *)dhead , dhead->length);
	
	size = snprintf(buff, 21, "%d", 1);
    size = write(uio_cfg.ctl_fd, buff, size);
    if (size <= 0)
		uio_error("failt write uio\n");
    return 0;
}
