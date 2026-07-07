override ARCH := x86
override ARCH_CFLAGS += -m32 -fno-stack-protector -nostdlib -ffreestanding  -fpack-struct \
    -mno-sse \
    -mno-sse2 \
    -mno-mmx \
    -mno-3dnow \
    -mno-avx \
    -mfpmath=387
override ARCH_ASFLAGS += -m32 -fno-stack-protector -nostdlib -ffreestanding
override ARCH_KLDFLAGS += -melf_i386
override ARCH_OUTPUT_SUFFIX := _x86.o
override ARCH_LINKER_SCRIPT := src/arch/x86/linker.ld
override ARCH_RUN_SCRIPT := src/arch/x86/run.sh
override KCC = clang

MODULE_NAMES += pci ide fat shell hello ps2 vga
