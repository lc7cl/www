
ifeq ($(RTE_SDK),)
	$(error "Must define RTE_SDK")
endif

ifeq ($(RTE_TARGET),)
	$(error "Must define RTE_SDK")
endif

include $(RTE_SDK)/mk/rte.vars.mk


CFLAGS += -g
CFLAGS += -I/home/lee/code/www/dpdk_app/tests/../dist/include 
CFLAGS += -I/home/lee/code/www/dpdk_app/tests/../dist/include/net
CFLAGS += -I/home/lee/code/www/dpdk_app/tests/../dist/include/dns 
CFLAGS += -L/home/lee/code/www/dpdk_app/tests/../dist/lib
ifeq ($(TRACE_MBUF),1)
CFLAGS += -DTRACE_MBUF
endif
LDFLAGS += -lnetproto
ifeq ($(TRACE_MBUF),1)
LDFLAGS += -ldns
endif
APP = test_udp
    
SRCS-y += test_udp.c
        
include $(RTE_SDK)/mk/rte.extapp.mk
