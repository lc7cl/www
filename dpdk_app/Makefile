
ifeq ($(RTE_SDK),)
	$(error "Must define RTE_SDK")
endif

ifeq ($(RTE_TARGET),)
	$(error "Must define RTE_SDK")
endif

include $(RTE_SDK)/mk/rte.vars.mk
include config
DPDK_APP_OUTPUT = $(shell pwd)/dist
O = $(shell pwd)/dist
export

DIRS-y += lib
DIRS-y += tests

include $(RTE_SDK)/mk/rte.extsubdir.mk
