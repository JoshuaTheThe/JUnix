#!/bin/bash
clear

if grub-file --is-x86-multiboot2 bin/boot/junix_x86.o; then
    echo "the file is multiboot"
    mkdir mnt
    mkdir bin/boot/grub -p
    cp src/arch/x86/grub.cfg bin/boot/grub/grub.cfg
    
    # Create HDD image instead of ISO
    dd if=/dev/zero of=junix_x86.hdd bs=1M count=32
    
    # Partition and format (using parted and mkfs)
    parted -s junix_x86.hdd mklabel msdos
    parted -s junix_x86.hdd mkpart primary fat32 1MiB 100%
    
    # Setup loop device WITH PARTITION SCANNING
    LOOP=$(sudo losetup --show -f --partscan junix_x86.hdd)
    echo "Loop device: ${LOOP}"
    echo "Partition: ${LOOP}p1"
    
    # Format the partition
    sudo mkfs.fat -F32 "${LOOP}p1"
    
    # Mount
    sudo mount "${LOOP}p1" mnt
    
    # Install GRUB to HDD
    sudo grub-install --target=i386-pc --root-directory=mnt --boot-directory=mnt/boot ${LOOP}
    
    sudo cp -r bin/* mnt/
    sudo cp -r misc/* mnt/

    #sudo mkdir -p mnt/src/arch/ mnt/src/kernel
    #sudo cp -r src/arch/* mnt/src/arch
    #sudo cp -r src/kernel/* mnt/src/kernel
    
    # Cleanup
    sudo umount mnt
    sudo losetup -d ${LOOP}
    
    # Run QEMU with HDD
    qemu-system-x86_64 \
    -drive file=junix_x86.hdd,if=ide,index=0,format=raw \
    -m 16 \
    -debugcon stdio \
    -no-reboot \
    -no-shutdown
else
    echo "the file is not multiboot"
fi
