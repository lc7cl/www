
ifeq ($(RTE_SDK),)
	$(error "Must define RTE_SDK")
endif

ifeq ($(RTE_TARGET),)
	$(error "Must define RTE_SDK")
endif

include $(RTE_SDK)/mk/rte.vars.mk

export DPDK_APP_OUTPUT = $(shell pwd)/dist

DIRS-y += lib
DIRS-y += tests

include $(RTE_SDK)/mk/rte.extsubdir.mk
