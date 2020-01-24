PWD := $(shell pwd)
obj-m += CharacterDeviceDriver.o

all:
	make ARCH=riscv CROSS_COMPILE=$(CROSS) M=$(PWD) -C $(KERNEL) SUBDIRS=$(PWD) modules
clean:
	make -C $(KERNEL) SUBDIRS=$(PWD) clean
