
ifeq ($(RTE_SDK),)
	$(error "Must define RTE_SDK")
endif

ifeq ($(RTE_TARGET),)
	$(error "Must define RTE_SDK")
endif

include $(RTE_SDK)/mk/rte.vars.mk

CFLAGS+= -g
APP=test

SRCS-y:=ip.c
SRCS-y+=netif.c
SRCS-y+=packet.c
SRCS-y+=main.c

include $(RTE_SDK)/mk/rte.extapp.mk
