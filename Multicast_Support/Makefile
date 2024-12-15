# obj-m += check_kmulticast.o
obj-m += rdma_kmulticast_check.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean