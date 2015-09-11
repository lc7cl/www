include $(RTE_SDK)/mk/rte.vars.mk

RTE_OUTPUT = $(DPDK_APP_OUTPUT)

SRCS-y := recursion.c
SRCS-y += request.c
SRCS-y += nameserver.c
SRCS-y += query.c
SRCS-y += arvaddr.c

SYMLINK-y-include/dns/recursion := librecursion.h
SYMLINK-y-include/dns/recursion += nameserver.h
SYMLINK-y-include/dns/recursion += query.h
SYMLINK-y-include/dns/recursion += request.h
SYMLINK-y-include/dns/recursion += srvaddr.h

include $(RTE_SDK)/mk/rte.extlib.mk
