
ifeq ($(RTE_SDK),)
	$(error "Must define RTE_SDK")
endif

ifeq ($(RTE_TARGET),)
	$(error "Must define RTE_SDK")
endif

include $(RTE_SDK)/mk/rte.vars.mk

CFLAGS+= -g

APP=test_tcp
    
SRC-y+=*.c
        
include $(RTE_SDK)/mk/rte.extapp.mk
