# riscv64-sample
build opensbi + qemu + linux for riscv



## Pre-requirement

software package

> sudo apt-get install git libglib2.0-dev libfdt-dev libpixman-1-dev zlib1g-dev

assuming the $WORK as the working directory

### Get Source and Build

build qemu (minimum version 4.1)
 
> cd $WORK

> git clone https://github.com/qemu/qemu.git

> cd qemu && git submodule update --init --recursive

> mkdir build && cd build

> ../configure --prefix=$PWD/../inst --target-list="riscv32-softmmu,riscv64-softmmu,riscv32-linux-user,riscv64-linux-user"    #build 32/64 user/system mode

> make -j 16 install 

> export PATH="$PWD/../inst/bin:$PATH" 

Freedum-u-sdk (for root file system)

> cd $WORK

> git clone https://github.com/sifive/freedom-u-sdk.git

> cd freedom-u-sdk

> git checkout -m origin/archive/buildroot

> git submodule update --init --recursive

> make initrd 

riscv-toolchain

> cd $WORK

> git clone --recursive https://github.com/riscv/riscv-gnu-toolchain

> cd riscv-gnu-toolchain

> ./configure --prefix=$WORK

> make newlib -j $(nproc)

> make linux -j $(nproc)

> export PATH="$PWD/../bin:$PATH" 


linux kernal 

> cd $WORK

> git clone https://github.com/torvalds/linux.git

> cd linux   

> make -j 16 ARCH=riscv CROSS_COMPILE=riscv64-unknown-linux-gnu- defconfig

> make -j 16 ARCH=riscv CROSS_COMPILE=riscv64-unknown-linux-gnu- vmlinux dtbs Image

opensbi

> cd $WORK

> git clone https://github.com/riscv/opensbi.git

> cd opensbi 

> make PLATFORM=qemu/virt FW_PAYLOAD_PATH=../linux/arch/riscv/boot/Image CROSS_COMPILE=riscv64-unknown-elf- I=inst


### RUN

> qemu-system-riscv64 -nographic -M virt -m 256M \\ \
-bios $WORK/opensbi/build/platform/qemu/virt/firmware/fw_jump.elf \\ \
-kernel $WORK/linux/arch/riscv/boot/Image \\ \
-initrd $WORK/freedom-u-sdk/work/initramfs.cpio.gz

### RUN Qemu and access it through ssh and scp

#### run qemu on server

> qemu-system-riscv64 -nographic -M virt -m 256M \\ \
-bios $PWD/opensbi-build/platform/qemu/virt/firmware/fw_jump.elf \\ \
-kernel $PWD/Image \\ \
-netdev user,id=eno4,hostfwd=tcp::32222-$GUEST_IP:22,hostfwd=tcp::22323-$GUEST_IP:23,hostfwd=tcp::26868-$GUEST_IP:68,hostfwd=tcp::28088-$GUEST_IP:80 \\ \
-device virtio-net-device,netdev=eno4 \\ \
-initrd $PWD/initramfs.cpio.gz

The -netdev options enables the network for qemu, for ssh we have forwarded host port number 32222 "hostfwd=tcp::32222-$GUEST_IP:22"

username: root
password: sifive
#### access qemu through ssh 

> ssh root@172.16.35.108 -p 32222

password: sifive

#### use scp to copy from your computer to a (remote) qemu server

> scp -P 32222 test.txt  root@172.16.35.108:/root

password: sifive

this command will copy file test.txt from local computer to remote qemu virtual machine

### modify initramfs.cpio.gz (rootfs)

extract initramfs.cpio.gz

> gzip -d initramfs.cpio.gz \
> cpio -idmv --no-absolute-filenames < initramfs.cpio.gz

OR

> gzip -d initramfs.cpio.gz \
> cpio -idm < initramfs.cpio

Insert your code in rootfs
after inserting your code create modified initramfs.cpio.gz named ramdisk.cpio.gz or  new_initramfs.cpio.gz

> sh -c ' find . | cpio -H newc -o' | gzip -9 > new_initramfs.cpio.gz

OR

> find . -print |cpio -H newc -o |gzip -9 > ../ramdisk.cpio.gz

while running qemu use ramdisk.cpio.gz or new_initramfs.cpio.gz instead of initramfs.cpio.gz

### for cross compiling of linux kernal module refer

https://blukat29.github.io/2017/12/cross-compile-arm-kernel-module/

#### create Makefile to cross compile kernel module

write the following in makefile

```
 PWD := $(shell pwd)
 obj-m += CharacterDeviceDriver.o
 
 all: 
        make ARCH=riscv CROSS_COMPILE=$(CROSS) M=$(PWD) -C $(KERNEL) SUBDIRS=$(PWD) modules
clean: 
        make -C $(KERNEL) SUBDIRS=$(PWD) clean
```

#### cross compile kernel module using following command

> export PATH="$PWD/../../../../work/bin:$PATH"

export the path where your risv-gnu-toolchain is installed 

> make KERNEL=../../../../work/linux/ CROSS=riscv64-unknown-linux-gnu-
