#!/bin/bash

# linux 4.13.9
export LINUX_OVERRIDE_SRCDIR=~/Documents/LabSisop/linux-4.13.9/

# recompile qemu/buildroot
make

# run
qemu-system-i386 \
    --device e1000,netdev=eth0,mac=aa:bb:cc:dd:ee:ff \
	--netdev tap,id=eth0,script=custom-scripts/qemu-ifup \
	--kernel output/images/bzImage \
	--hda output/images/rootfs.ext2 \
	--nographic \
	--append "console=ttyS0 root=/dev/sda"
