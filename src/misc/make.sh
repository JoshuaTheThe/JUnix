
clang -m32 init.c -o init.o -nostdlib -nostartfiles -c
ld init.o -o init -melf_i386
