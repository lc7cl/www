include $(RTE_SDK)/mk/rte.vars.mk

ifeq ($(RTE_OUTPUT),)
$(error "RTE_OUTPUT undefined")
endif

RTE_OUTPUT = $(DPDK_APP_OUTPUT)

# library name
LIB = libnetproto.a

CFLAGS += -O3
CFLAGS += $(WERROR_FLAGS) -I$(SRCDIR)

EXPORT_MAP := netproto_version.map

LIBABIVER := 1

# DIRS-y := common

# all source are stored in SRCS-y
SRCS-y := af_inet.c
SRCS-y += arp.c
SRCS-y += buffer.c
SRCS-y += hook.c
SRCS-y += netdev.c
SRCS-y += netif.c
SRCS-y += packet.c
SRCS-y += port_queue_map.c
SRCS-y += sk.c
SRCS-y += tcp.c
SRCS-y += udp.c

# install this header file
SYMLINK-y-include := af_inet.h
SYMLINK-y-include += arp.h
SYMLINK-y-include += buffer.h
SYMLINK-y-include += hook.h
SYMLINK-y-include += netdev.h
SYMLINK-y-include += netif.h
SYMLINK-y-include += packet.h
SYMLINK-y-include += port_queue_map.h
SYMLINK-y-include += sk.h
SYMLINK-y-include += tcp.h
SYMLINK-y-include += udp.h
SYMLINK-y-include/common := common/common.h

# this lib needs eal and ring

include $(RTE_SDK)/mk/rte.extlib.mk
