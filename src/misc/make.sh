
clang -m32 init.c stdio.c -o init.o -nostdlib -nostartfiles -c -I .
ld init.o -o init -melf_i386
