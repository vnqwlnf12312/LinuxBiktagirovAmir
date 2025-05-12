#!/bin/bash

cd root
find . | cpio -ov --format=newc | gzip -9 > ../initramfs
cd ..

#!/bin/bash

if [[ "$VIRTUALIZATION_FLAGS" == "true" ]]; then
  FLAGS="-cpu host --enable-kvm"
else
  FLAGS=""
fi

qemu-system-x86_64 $FLAGS -kernel ./boot/vmlinuz-6.14.6 -initrd initramfs -nographic -append "console=ttyS0"