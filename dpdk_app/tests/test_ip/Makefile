
ifeq ($(RTE_SDK),)
	$(error "Must define RTE_SDK")
endif

ifeq ($(RTE_TARGET),)
	$(error "Must define RTE_SDK")
endif

include $(RTE_SDK)/mk/rte.vars.mk



CFLAGS += -g -I/home/lee/code/www/dpdk_app/tests/../dist/include -I/home/lee/code/www/dpdk_app/tests/../dist/include/net -L/home/lee/code/www/dpdk_app/tests/../dist/lib -lnetproto
APP = test_ip
    
SRCS-y += test_ip.c
        
include $(RTE_SDK)/mk/rte.extapp.mk
