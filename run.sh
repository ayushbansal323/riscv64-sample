export PATH="$PWD/qemu-riscv:$PATH"
qemu-system-riscv64 -nographic -M virt -m 256M \
-bios $PWD/opensbi-build/platform/qemu/virt/firmware/fw_jump.elf \
-kernel $PWD/Image \
-initrd $PWD/ramdisk.cpio.gz
