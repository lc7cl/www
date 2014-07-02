objs-m += modify_pkt.o
KERNEL_PATH = /lib/modules/$(shell uname -r)/build
#KERNEL_PATH = /usr/src/linux
CUR=$(shell pwd)

default:
	make -C $(KERNEL_PATH)  M=$(CUR) modules 

clean:
	make -C $(KERNEL_PATH) M=$(CUR) clean
