        .section .text
        .global timer_int
        .global default_int
        .global sys_wrapper
        .global ide_int
        .extern trap_next
        .extern sys_handler
        .extern IDEIrq
        .extern current_task
default_int:
        cli
        jmp default_int
timer_int:
        cli
        jmp trap_next
ide_int:
        cli
        jmp IDEIrq
        sti
        iret
sys_wrapper:
        cli
        movl    %eax, (0*4+scratch)
        movl    %ecx, (1*4+scratch)
        movl    %edx, (2*4+scratch)
        movl    %ebx, (3*4+scratch)
        movl    %ebp, (5*4+scratch)
        movl    %esi, (6*4+scratch)
        movl    %edi, (7*4+scratch)
        movl    $scratch, %edi
        popl    8*4(%edi)
        popl    9*4(%edi)
        popl    15*4(%edi)
        movl    %esp, (4*4+scratch)

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
        # --- first dword of current_task is a pointer to the kernel stack
        movl    (current_task), %esp
        movl    (%esp), %esp
        xorl    %ebp,%ebp

        call    sys_handler

        # --- RELOAD POINTER (calls may have trashed edi) ---
        movl    $scratch, %edi

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
        pushl   15*4+scratch
        pushl   9*4+scratch
        pushl   8*4+scratch
        sti
        iret
