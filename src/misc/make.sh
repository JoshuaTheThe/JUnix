
clang -m32 init.c -o init.o -nostdlib -nostartfiles -c -I .
clang -m32 stdio.c -o stdio.o -nostdlib -nostartfiles -c -I .
ld init.o stdio.o -o init -melf_i386
