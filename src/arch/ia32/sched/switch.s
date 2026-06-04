        .section .text
        .global trap_next
        .extern scratch_proc
        .extern scheduler_next
        .extern scheduler_save
        .extern scheduler_load
        .extern ticks_since_boot
trap_next:
        cli
        movl    %eax, (0*4+scratch_proc)
        movl    %ecx, (1*4+scratch_proc)
        movl    %edx, (2*4+scratch_proc)
        movl    %ebx, (3*4+scratch_proc)
        movl    %ebp, (5*4+scratch_proc)
        movl    %esi, (6*4+scratch_proc)
        movl    %edi, (7*4+scratch_proc)
        movl    $scratch_proc, %edi
        popl    8*4(%edi)
        popl    9*4(%edi)
        popl    15*4(%edi)
        movl    %esp, (4*4+scratch_proc)

        # --- SEGMENT REGISTERS ---
        movw    %ds, %ax
        movw    %es, %bx
        movw    %ss, %cx
        movw    %fs, %dx
        movw    %gs, %si
        movl    %eax, 10*4(%edi)
        movl    %ebx, 11*4(%edi)
        movl    %ecx, 12*4(%edi)
        movl    %edx, 13*4(%edi)
        movl    %esi, 14*4(%edi)

        movw    $0x10, %ax
        movw    %ax, %ds
	movw    %ax, %es
	movw    %ax, %ss
	movw    %ax, %fs
	movw    %ax, %gs

        # --- SWITCH TO INTERRUPT STACK AND COMMIT ---
        movl    $(kstack_top-16), %esp
        xorl    %ebp,%ebp

        call    scheduler_save
        call    scheduler_next
        call    scheduler_load
        incl    (ticks_since_boot)

        # --- RELOAD POINTER (calls may have trashed edi) ---
        movl    $scratch_proc, %edi

        # --- RESTORE SEGMENT REGISTERS ---
        movl    10*4(%edi), %eax
        movl    11*4(%edi), %ebx
        movl    12*4(%edi), %ecx
        movl    13*4(%edi), %edx
        movl    14*4(%edi), %esi
        movw    %ax, %ds
        movw    %bx, %es
        movw    %cx, %ss
        movw    %dx, %fs
        movw    %si, %gs

        orl     $0x0202, 15*4(%edi)

        # --- RESTORE GPRS (edi last) ---
        movl    0*4(%edi), %eax
        movl    1*4(%edi), %ecx
        movl    2*4(%edi), %edx
        movl    3*4(%edi), %ebx
        movl    4*4(%edi), %esp
        movl    5*4(%edi), %ebp
        movl    6*4(%edi), %esi
        movl    7*4(%edi), %edi
        pushl   15*4+scratch_proc
        pushl   9*4+scratch_proc
        pushl   8*4+scratch_proc
        sti
        iret

        .section .bss
kstack:
        .space  4096
kstack_top:
