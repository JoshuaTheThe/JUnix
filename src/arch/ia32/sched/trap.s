        .section .text
        .global timer_int
        .global default_int
        .extern next_process
timer_int:
        cli
        jmp next_process
default_int:
        cli
        jmp default_int