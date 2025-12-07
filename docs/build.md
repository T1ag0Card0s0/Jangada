# To build

```bash
make ARCH=aarch64 BOARD=qemu_virt TRIPLET=aarch64-none-elf-
```
Jangada RTOS build structure
```
/build
    /jangada
        /bsps
            libbsp.a
        /kernel
            libjangada.a
        /libc
            libc.a
    /sysroot
        /boot
        /usr
            /include
            /lib
                libbsp.a
                libc.a
                libjangada.a
```