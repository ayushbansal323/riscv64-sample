GUEST_IP=10.0.2.15
export PATH="$PWD/qemu-riscv:$PATH"
qemu-system-riscv64 -nographic -M virt -m 256M \
-bios $PWD/opensbi-build/platform/qemu/virt/firmware/fw_jump.elf \
-kernel $PWD/Image \
-netdev user,id=eno4,hostfwd=tcp::32222-$GUEST_IP:22,hostfwd=tcp::22333-$GUEST_IP:23,hostfwd=tcp::2444-$GUEST_IP:443,hostfwd=tcp::26866-$GUEST_IP:68,hostfwd=tcp::28888-$GUEST_IP:80 \
-device virtio-net-device,netdev=eno4 \
-initrd $PWD/ramdisk.cpio.gz
