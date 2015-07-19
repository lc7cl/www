
ifeq ($(RTE_SDK),)
	$(error "Must define RTE_SDK")
endif

ifeq ($(RTE_TARGET),)
	$(error "Must define RTE_SDK")
endif

include $(RTE_SDK)/mk/rte.vars.mk

DIRS-y += lib
DIRS-y += tests

include $(RTE_SDK)/mk/rte.extsubdir.mk
