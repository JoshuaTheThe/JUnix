ENTRY(_start)

SECTIONS
{
        KERNEL_VMA = 0xC0000000;
        KERNEL_LMA = 0x00200000;
        KERNEL_RELOC = 0xC0000000 - 0x00200000;
        . = KERNEL_VMA;

        .text : AT(ADDR(.text) - KERNEL_VMA + KERNEL_LMA)
        {
                *(.multiboot)
                *(.multiboot2)
                *(.text*)
        }

        .rodata : AT(ADDR(.rodata) - KERNEL_VMA + KERNEL_LMA)
        {
                *(.rodata*)
        }

        .data : AT(ADDR(.data) - KERNEL_VMA + KERNEL_LMA)
        {
                *(.data*)
                *(.got*)
                *(.plt*)
                *(.iplt*)
        }

        .bss : AT(ADDR(.bss) - KERNEL_VMA + KERNEL_LMA)
        {
                *(COMMON)
                *(.bss*)
        }

        . = ALIGN(4096);

        stack_bottom = .;
        . += 0x100000;
        stack_top = .;

        /DISCARD/ : {
                *(.note.GNU-stack)
                *(.gnu_debuglink)
                *(.gnu.lto_*)
                *(.comment*)
        }

        mem_start   = .;
        page_bitmap = 0x1000;
}
