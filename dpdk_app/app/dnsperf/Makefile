include $(RTE_SDK)/mk/rte.vars.mk

ifeq ($(RTE_OUTPUT),)
$(error "RTE_OUTPUT undefined")
endif

RTE_OUTPUT = $(DPDK_APP_OUTPUT)

# library name
APP = dnsperf

CFLAGS += -O3 -g -DCONFIG_DNS 

SRCS-y := main.c

include $(RTE_SDK)/mk/rte.extapp.mk
