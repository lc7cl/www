#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef __cpluscplus
extern "C" {
#endif

#include <rte_log.h>
#include <linux/uio.h>

typedef uint32_t be32;

#define __in
#define __out
#define RTE_LOGTYPE_NET RTE_LOGTYPE_USER1+1

typedef struct msg_hdr {
	struct ctl_hdr {
		int type;
	} ctlhdr;
	struct iovec *iov;
	int iov_length;
} msg_hdr_t;

#ifdef __cplusplus
}
#endif

#endif