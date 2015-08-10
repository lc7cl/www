#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef __cpluscplus
extern "C" {
#endif

#include <rte_log.h>

#define __in
#define __out
#define RTE_LOGTYPE_NET RTE_LOGTYPE_USER1+1

#ifdef __cplusplus
}
#endif

#endif