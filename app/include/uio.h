#ifndef _UIO_H
#define _UIO_H

#include <stddef.h>

#define FLAG_ADD 1
#define FLAG_DEL 2
#define FLAG_SEARCH 4

struct download_head
{
	int flag;
#define DOWNLOAD_ADD 1
#define DOWNLOAD_DEL 2
#define DOWNLOAD_SEARCH 4
	int length;
};

extern int open_uio();
extern void close_uio();
extern int write_uio(int flag, char *data, size_t len);

#endif
