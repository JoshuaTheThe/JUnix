        .section .text
        .global timer_int
        .global default_int
        .extern trap_next
timer_int:
        cli
        jmp trap_next
default_int:
        cli
        jmp default_int
