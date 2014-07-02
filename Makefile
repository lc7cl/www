objs-m += modify_pkt.o
KERNEL_PATH = /lib64/modules/$(shell uname -r)/build
#KERNEL_PATH = /usr/src/linux
CUR=$(shell pwd)

default:
	make -C $(KERNEL_PATH) V=1  M=$(CUR) modules

clean:
	make -C $(KERNEL_PATH) M=$(CUR) clean
