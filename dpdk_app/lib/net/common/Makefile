include $(RTE_SDK)/mk/rte.vars.mk

ifeq ($(RTE_OUTPUT),)
$(error "RTE_OUTPUT undefined")
endif

RTE_OUTPUT = $(DPDK_APP_OUTPUT)

# library name

# all source are stored in SRCS-y

# install this header file
SYMLINK-y-include/common := common.h

# this lib needs eal and ring

include $(RTE_SDK)/mk/rte.extlib.mk
