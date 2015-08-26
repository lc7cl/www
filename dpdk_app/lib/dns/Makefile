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

EXPORT_MAP := dns_version.map

LIBABIVER := 1

# DIRS-y := common

# all source are stored in SRCS-y
SRCS-y := common/dns.c
SRCS-y += common/utils.c

# install this header file
SYMLINK-y-include/dns/common := common/dns.h
SYMLINK-y-include/dns/common += common/utils.h

# this lib needs eal and ring

include $(RTE_SDK)/mk/rte.extlib.mk
