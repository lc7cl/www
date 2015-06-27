
ifeq ($(RTE_SDK),)
	$(error "Must define RTE_SDK")
endif

RTE_TARGET ?= x86_64-native-linuxapp-gcc

include $(RTE_SDK)/mk/rte.vars.mk

CFLAGS+= -g
APP=test

#SRCS-y:=test.c
SRCS-y+=ip.c

include $(RTE_SDK)/mk/rte.extapp.mk
