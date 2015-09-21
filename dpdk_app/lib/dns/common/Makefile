include $(RTE_SDK)/mk/rte.vars.mk

RTE_OUTPUT = $(DPDK_APP_OUTPUT)

SRCS-y := dns.c
SRCS-y += dns_memory.c
SRCS-y += hash.c
SRCS-y += message.c
SRCS-y += name.c
SRCS-y += utils.c

SYMLINK-y-include/dns/common := dns.h
SYMLINK-y-include/dns/common += dns_memory.h
SYMLINK-y-include/dns/common += hash.h
SYMLINK-y-include/dns/common += message.h
SYMLINK-y-include/dns/common += name.h
SYMLINK-y-include/dns/common += utils.h

include $(RTE_SDK)/mk/rte.extlib.mk
