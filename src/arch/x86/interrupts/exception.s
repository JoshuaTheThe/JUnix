        .section .text
        .global divide_error
        .global debug_exception
        .global nmi_exception
        .global breakpoint_exception
        .global overflow_exception
        .global bound_exception
        .global invalid_opcode
        .global device_not_available
        .global double_fault
        .global invalid_tss
        .global segment_not_present
        .global stack_fault
        .global general_protection
        .global page_fault
        .global x87_exception
        .global alignment_check
        .global machine_check
        .global simd_exception
        .extern divide_error_handler
        .extern debug_exception_handler
        .extern nmi_exception_handler
        .extern breakpoint_exception_handler
        .extern overflow_exception_handler
        .extern bound_exception_handler
        .extern invalid_opcode_handler
        .extern device_not_available_handler
        .extern double_fault_handler
        .extern invalid_tss_handler
        .extern segment_not_present_handler
        .extern stack_fault_handler
        .extern general_protection_handler
        .extern page_fault_handler
        .extern x87_exception_handler
        .extern alignment_check_handler
        .extern machine_check_handler
        .extern simd_exception_handler
divide_error:
        cli
        call divide_error_handler
1:      jmp 1b
debug_exception:
        cli
        call debug_exception_handler
1:      jmp 1b
nmi_exception:
        cli
        call nmi_exception_handler
1:      jmp 1b
breakpoint_exception:
        cli
        call breakpoint_exception_handler
1:      jmp 1b
overflow_exception:
        cli
        call overflow_exception_handler
1:      jmp 1b
bound_exception:
        cli
        call bound_exception_handler
1:      jmp 1b
invalid_opcode:
        cli
        call invalid_opcode_handler
1:      jmp 1b
device_not_available:
        cli
        call device_not_available_handler
1:      jmp 1b
double_fault:
        cli
        call double_fault_handler
1:      jmp 1b
invalid_tss:
        cli
        call invalid_tss_handler
1:      jmp 1b
segment_not_present:
        cli
        call segment_not_present_handler
1:      jmp 1b
stack_fault:
        cli
        call stack_fault_handler
1:      jmp 1b
general_protection:
        cli
        call general_protection_handler
1:      jmp 1b
page_fault:
        cli
        call page_fault_handler
1:      jmp 1b
x87_exception:
        cli
        call x87_exception_handler
1:      jmp 1b
alignment_check:
        cli
        call alignment_check_handler
1:      jmp 1b
machine_check:
        cli
        call machine_check_handler
1:      jmp 1b
simd_exception:
        cli
        call simd_exception_handler
1:      jmp 1b
