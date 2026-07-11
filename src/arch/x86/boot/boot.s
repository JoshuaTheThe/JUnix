        .section .multiboot
        .align 8
        .section .multiboot
        .align 8
multiboot_header:
        .long 0xE85250D6
        .long 0
        .long (multiboot_header_end - multiboot_header)
        .long -(0xE85250D6 + 0 + (multiboot_header_end - multiboot_header))

        /* Information request tag (type 1) */
        .align 8
        .word 1                    /* type */
        .word 0                    /* flags */
        .long 8 + 12               /* size: 8 header + 3*4 */
        .long 0                    /* request basic info (tag type 0) */
        .long 6                    /* request memory map (tag type 6) */
        .long 8                    /* request framebuffer info (tag type 8) */
        .long 0                    /* terminator */

        /* Console tag (type 4), optional */
        .align 8
        .word 4                    /* type */
        .word 0                    /* flags */
        .long 8                    /* size */

        /* Framebuffer tag (type 5) */
        .align 8
        .word 5                    /* type */
        .word 1                    /* flags (0=linear, 1=preferred) */
        .long 20                   /* size: 20 bytes total */
        .long 1024                 /* width */
        .long 768                  /* height */
        .long 32                   /* depth */

        /* End tag (type 0) */
        .align 8
        .word 0
        .word 0
        .long 8
multiboot_header_end:
        .section .text
        .global _start
        .global boot_page_directory
        .global boot_page_table1
        .global boot_page_table2
        .global boot_page_table3
        .global boot_page_table4
        .global boot_page_table_low
        .type _start, @function
        .equ KERNEL_RELOC, (0xC0000000 - 0x00200000)
        .extern init
_start:
        movl %eax, %edx
        #
        # Clear page directory
        #
        mov $(boot_page_directory - KERNEL_RELOC), %edi
        xor %eax, %eax
        mov $1024, %ecx
        rep stosl


        #
        # Install PDEs
        #

        mov $(boot_page_table1 - KERNEL_RELOC), %eax
        or $0x3, %eax
        mov %eax, (boot_page_directory - KERNEL_RELOC + 768*4)


        mov $(boot_page_table2 - KERNEL_RELOC), %eax
        or $0x3, %eax
        mov %eax, (boot_page_directory - KERNEL_RELOC + 769*4)


        mov $(boot_page_table3 - KERNEL_RELOC), %eax
        or $0x3, %eax
        mov %eax, (boot_page_directory - KERNEL_RELOC + 770*4)


        mov $(boot_page_table4 - KERNEL_RELOC), %eax
        or $0x3, %eax
        mov %eax, (boot_page_directory - KERNEL_RELOC + 771*4)



        #
        # Fill PT1: 0xC0000000 -> 0x00200000
        #

        mov $(boot_page_table1 - KERNEL_RELOC), %edi
        mov $0x00200000, %eax
        mov $1024, %ecx

.fill_pt1:
        mov %eax, (%edi)
        orl $3, (%edi)
        add $0x1000, %eax
        add $4, %edi
        loop .fill_pt1



        #
        # Fill PT2: 0xC0400000 -> 0x00600000
        #

        mov $(boot_page_table2 - KERNEL_RELOC), %edi
        mov $0x00600000, %eax
        mov $1024, %ecx

.fill_pt2:
        mov %eax, (%edi)
        orl $3, (%edi)
        add $0x1000, %eax
        add $4, %edi
        loop .fill_pt2



        #
        # Fill PT3: 0xC0800000 -> 0x00A00000
        #

        mov $(boot_page_table3 - KERNEL_RELOC), %edi
        mov $0x00A00000, %eax
        mov $1024, %ecx

.fill_pt3:
        mov %eax, (%edi)
        orl $3, (%edi)
        add $0x1000, %eax
        add $4, %edi
        loop .fill_pt3



        #
        # Fill PT4: 0xC0C00000 -> 0x00E00000
        #

        mov $(boot_page_table4 - KERNEL_RELOC), %edi
        mov $0x00E00000, %eax
        mov $1024, %ecx

.fill_pt4:
        mov %eax, (%edi)
        orl $3, (%edi)
        add $0x1000, %eax
        add $4, %edi
        loop .fill_pt4

        #
        # Low identity map
        # 0x00000000 -> 0x00000000
        #

        mov $(boot_page_table_low - KERNEL_RELOC), %eax
        orl $3, %eax
        mov %eax, (boot_page_directory - KERNEL_RELOC + 0)


        mov $(boot_page_table_low - KERNEL_RELOC), %edi
        xor %eax, %eax
        mov $1024, %ecx

.fill_low:
        mov %eax, (%edi)
        orl $3, (%edi)
        add $0x1000, %eax
        add $4, %edi
        loop .fill_low

        #
        # Load CR3 (physical address)
        #

        mov $(boot_page_directory - KERNEL_RELOC), %eax
        mov %eax, %cr3

        #
        # Enable paging
        #

        mov %cr0, %eax
        or $0x80000000, %eax
        mov %eax, %cr0

        #
        # Now virtual addresses work
        #

        lea higher_half_entry, %eax
        jmp *%eax
higher_half_entry:
        mov $stack_top, %esp
        xor %ebp, %ebp

        push %ebx
        push %edx
        call init
        call kmain
1:      cli
        hlt
        jmp 1b
        .size _start, . - _start
        .section .bss
        .align 4096
boot_page_directory:
        .skip 4096
boot_page_table_low:
        .skip 4096
boot_page_table1:
        .skip 4096
boot_page_table2:
        .skip 4096
boot_page_table3:
        .skip 4096
boot_page_table4:
        .skip 4096
