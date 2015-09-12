include $(RTE_SDK)/mk/rte.vars.mk

ifeq ($(RTE_OUTPUT),)
$(error "RTE_OUTPUT undefined")
endif

RTE_OUTPUT = $(DPDK_APP_OUTPUT)

# library name
LIB = libdns.a

CFLAGS += -O3 -g 
ifeq ($(TRACE_MBUF),1)
	CFLAGS += -DTRACE_MBUF
endif
CFLAGS += $(WERROR_FLAGS) -I$(SRCDIR)
ifeq ($(CONFIG_DNS_CACHE),y)
	CFLAGS += -DCONFIG_DNS_CACHE
endif

EXPORT_MAP := dns_version.map

LIBABIVER := 1

#DIRS-y := common
#DIRS-$(CONFIG_DNS_CACHE) += cache
#DIRS-$(CONFIG_DNS_RECURSION) += recursion

# all source are stored in SRCS-y
SRCS-y := common/dns.c
SRCS-y += common/utils.c
SRCS-y += common/dns_memory.c
SRCS-y += common/hash.c
SRCS-y += common/message.c
SRCS-y += common/name.c

SRCS-$(CONFIG_DNS_RECURSION) += recursion/recursion.c
#SRCS-$(CONFIG_DNS_RECURSION) += recursion/request.c
#SRCS-$(CONFIG_DNS_RECURSION) += recursion/nameserver.c
SRCS-$(CONFIG_DNS_RECURSION) += recursion/query.c
SRCS-$(CONFIG_DNS_RECURSION) += recursion/srvaddr.c
SRCS-$(CONFIG_DNS_RECURSION) += recursion/client.c

SRCS-$(CONFIG_DNS_CACHE) += cache/dns_cache.c


# install this header file
SYMLINK-y-include/dns/common := common/dns.h
SYMLINK-y-include/dns/common += common/utils.h
SYMLINK-y-include/dns/common += common/dns_memory.h
SYMLINK-y-include/dns/common += common/hash.h
SYMLINK-y-include/dns/common += common/message.h
SYMLINK-y-include/dns/common += common/name.h

SYMLINK-$(CONFIG_DNS_RECURSION)-include/dns/recursion := recursion/librecursion.h
SYMLINK-$(CONFIG_DNS_RECURSION)-include/dns/recursion += recursion/nameserver.h
SYMLINK-$(CONFIG_DNS_RECURSION)-include/dns/recursion += recursion/query.h
SYMLINK-$(CONFIG_DNS_RECURSION)-include/dns/recursion += recursion/request.h
SYMLINK-$(CONFIG_DNS_RECURSION)-include/dns/recursion += recursion/srvaddr.h
SYMLINK-$(CONFIG_DNS_RECURSION)-include/dns/recursion += recursion/client.h

SYMLINK-$(CONFIG_DNS_CACHE)-include/dns/cache += cache/dns_cache.h


# this lib needs eal and ring

include $(RTE_SDK)/mk/rte.extlib.mk
