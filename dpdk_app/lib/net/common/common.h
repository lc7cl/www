#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef __cpluscplus
extern "C" {
#endif

#include <sys/uio.h>

#include <rte_log.h>
#include <rte_malloc.h>

typedef uint32_t be32;

#define __in
#define __out
#define RTE_LOGTYPE_PROTO (RTE_LOGTYPE_USER8+1)
#define RTE_LOGTYPE_MBUFF (RTE_LOGTYPE_USER8+2)
#define RTE_LOGTYPE_TEST  (RTE_LOGTYPE_USER8+8)


typedef struct msg_hdr {
	struct ctl_hdr {
		int type;
	} ctlhdr;
	struct iovec *iov;
	int iov_length;
} msg_hdr_t;

static inline struct iovec* msg_hdr_alloc_iovs(int iov_length, int data_len)
{
	int i;
    struct iovec *iov;

	iov = rte_malloc(NULL, iov_length * (sizeof(struct iovec) + data_len), 0);
	if (iov) {
		for (i = 0; i < iov_length; i++) {
			iov[i].iov_len = data_len;
			iov[i].iov_base = (void*)(((char*)iov) + 
				i *(sizeof(struct iovec) + data_len) + sizeof(struct iovec));
		}
	}		
	return iov;
}

#ifdef __cplusplus
}
#endif

#endif
