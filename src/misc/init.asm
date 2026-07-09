        section .text
        global _start
_start: 
        mov eax, 256
        mov ebx, _msg
        int 0x80
        jmp $
        section .data
        global _msg
_msg:   db "Hello world, From /mnt/init!\r\n", 0
_msgE:
