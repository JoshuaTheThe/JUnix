        .section .text
        .global SSEIsAvailable
        .global SSEEnable

# ====== FIXED: SSEIsAvailable ======
SSEIsAvailable:
    pushl %ebp
    movl %esp, %ebp
    pushl %ebx
    pushl %ecx
    pushl %edx
    
    movl $0x01, %eax
    cpuid
    
    # Check FPU (bit 0 in EDX)
    testl $0x01, %edx
    jz .notPresent
    
    # Check SSE (bit 25 in EDX)
    testl $0x02000000, %edx
    jz .notPresent
    
    # Check SSE2 (bit 26 in EDX)
    testl $0x04000000, %edx
    jz .notPresent
    
    # Check FXSR (bit 24 in EDX)
    testl $0x01000000, %edx
    jz .notPresent
    
    # All present!
    movl $0x01, %eax
    jmp .return
.notPresent:
    xorl %eax, %eax
.return:
    popl %edx
    popl %ecx
    popl %ebx
    movl %ebp, %esp
    popl %ebp
    ret

# ====== FIXED: SSEEnable ======
SSEEnable:
    pushl %ebp
    movl %esp, %ebp
    
    # Enable FPU and SSE in CR0
    movl %cr0, %eax
    andw $0xFFFB, %ax     # Clear EM (bit 2) - FPU emulation
    orw $0x0002, %ax      # Set MP (bit 1) - Monitor coprocessor
    movl %eax, %cr0
    
    # Enable SSE in CR4
    movl %cr4, %eax
    orw $(3 << 9), %ax    # Set OSFXSR (bit 9) and OSXMMEXCPT (bit 10)
    movl %eax, %cr4
    
    movl %ebp, %esp
    popl %ebp
    ret