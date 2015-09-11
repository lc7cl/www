#ifndef _LIBRECURSION_H_
#define _LIBRECURSION_H_

#include <recursion/query.h>

enum RET_RECURSION {
	RET_R_OK = 0,
	RET_R_FAIL,
	RET_R_UNSUPPORT,

	RET_R_MAX
};

int start_recursion(struct dns_query *query);

#endif
