.section .text
.global FeatureIsPresentEDX
.global FeatureIsPresentECX
.global FeatureCR4Enable
.global FeatureCR0Enable
.global GetCR3
.global SetCR3

# ====== FIXED: FeatureIsPresentEDX ======
FeatureIsPresentEDX:
    pushl %ebp
    movl %esp, %ebp
    
    movl 8(%ebp), %ecx    # Get bit index directly (no push/pop!)
    movl $0x01, %eax
    shl %cl, %eax         # Create mask
    
    pushl %ebx            # Save registers CPUID clobbers
    pushl %ecx
    pushl %edx
    
    movl $0x01, %eax
    cpuid
    
    testl %eax, %edx      # Test if bit is set in EDX
    popl %edx             # Restore
    popl %ecx
    popl %ebx
    
    jz .notPresent
    movl $0x01, %eax
    jmp .return
.notPresent:
    movl $0x00, %eax
.return:
    movl %ebp, %esp
    popl %ebp
    ret

# ====== FIXED: FeatureIsPresentECX ======
FeatureIsPresentECX:
    pushl %ebp
    movl %esp, %ebp
    
    movl 8(%ebp), %ecx
    movl $0x01, %eax
    shl %cl, %eax
    
    pushl %ebx
    pushl %ecx
    pushl %edx
    
    movl $0x01, %eax
    cpuid
    
    testl %eax, %ecx      # Test if bit is set in ECX
    popl %edx
    popl %ecx
    popl %ebx
    
    jz .notPresent2
    movl $0x01, %eax
    jmp .return2
.notPresent2:
    movl $0x00, %eax
.return2:
    movl %ebp, %esp
    popl %ebp
    ret

# ====== FIXED: FeatureCR0Enable ======
FeatureCR0Enable:
    pushl %ebp
    movl %esp, %ebp
    
    movl 8(%ebp), %ecx    # Bit index
    movl $0x01, %eax
    shl %cl, %eax         # Create mask
    
    movl %cr0, %edx
    orl %eax, %edx        # Set the bit
    movl %edx, %cr0
    
    movl %ebp, %esp
    popl %ebp
    ret

# ====== FIXED: FeatureCR4Enable ======
FeatureCR4Enable:
    pushl %ebp
    movl %esp, %ebp
    
    movl 8(%ebp), %ecx
    movl $0x01, %eax
    shl %cl, %eax
    
    movl %cr4, %edx
    orl %eax, %edx
    movl %edx, %cr4
    
    movl %ebp, %esp
    popl %ebp
    ret

# ====== FIXED: GetCR3 ======
GetCR3:
    pushl %ebp
    movl %esp, %ebp
    movl %cr3, %eax
    movl %ebp, %esp
    popl %ebp
    ret

# ====== FIXED: SetCR3 ======
SetCR3:
    pushl %ebp
    movl %esp, %ebp
    movl 8(%ebp), %eax    # Page directory address
    movl %eax, %cr3
    movl %ebp, %esp
    popl %ebp
    ret
