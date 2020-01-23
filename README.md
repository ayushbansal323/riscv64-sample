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


### modify initramfs.cpio.gz

extract initramfs.cpio.gz

> gzip -d initramfs.cpio.gz \
> cpio -idmv --no-absolute-filenames < initramfs.cpio.gz

OR

> gzip -d initramfs.cpio.gz
> cpio -idm < initramfs.cpio
insert your code in rootfs
after inserting your code create modified.initramfs.cpio.gz

> sh -c ' find . | cpio -H newc -o' | gzip -9 > new_initramfs.cpio.gz

OR

> find . -print |cpio -H newc -o |gzip -9 > ../ramdisk.cpio.gz


### for cross compiling of linux kernal module refer

https://blukat29.github.io/2017/12/cross-compile-arm-kernel-module/
