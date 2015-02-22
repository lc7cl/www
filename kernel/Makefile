ifneq ($(KERNELRELEASE),)

obj-m := test_memcmp.o
obj-m += test.o

else


MAKE = make
Q = 

KERNEL_DIR := /lib/modules/$(shell uname -r)/build
SRC_DIR := $(shell pwd)

default:
	$(Q)$(MAKE) -C $(KERNEL_DIR) M=$(SRC_DIR) modules

clean:
	$(Q)$(MAKE) -C $(KERNEL_DIR) M=$(SRC_DIR) clean


.PHONY:clean

endif
