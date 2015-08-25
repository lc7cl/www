
ifeq ($(RTE_SDK),)
	$(error "Must define RTE_SDK")
endif

ifeq ($(RTE_TARGET),)
	$(error "Must define RTE_SDK")
endif

include $(RTE_SDK)/mk/rte.vars.mk



CFLAGS += -g -I/home/lee/code/www/dpdk_app/tests/../dist/include -I/home/lee/code/www/dpdk_app/tests/../dist/include/net -I/home/lee/code/www/dpdk_app/tests/../dist/include/dns -L/home/lee/code/www/dpdk_app/tests/../dist/lib
LDFLAGS += -lnetproto
APP = test_udp
    
SRCS-y += test_udp.c
        
include $(RTE_SDK)/mk/rte.extapp.mk
