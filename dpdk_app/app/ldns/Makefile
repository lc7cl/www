include $(RTE_SDK)/mk/rte.vars.mk

ifeq ($(RTE_OUTPUT),)
$(error "RTE_OUTPUT undefined")
endif

RTE_OUTPUT = $(DPDK_APP_OUTPUT)

# library name
APP = ldns

CFLAGS += -O3 -g -DCONFIG_DNS 
CFLAGS += -I$(DPDK_APP_OUTPUT)/include -I$(DPDK_APP_OUTPUT)/include/net -I$(DPDK_APP_OUTPUT)/include/dns -L$(DPDK_APP_OUTPUT)/lib

LDFLAGS += -lnetproto -ldns

SRCS-y := ldns-main.c
SRCS-y += dispatch.c
SRCS-y += dns_conf.c

DEPDIRS-y += net
DEPDIRS-y += dns

include $(RTE_SDK)/mk/rte.extapp.mk
