# TODO
- things i need to do

## Paging
1. copy_from_user
2. copy_to_user

3. Switch PD during interrupts
   - or ensure kernel mappings exist in every address space

4. Move kernel to high memory
   - shared kernel mappings in every process PD
   - interrupt handlers always mapped

5. Kernel/user page permissions
   - add USER bit to user pages
   - kernel pages supervisor-only
   - make sure user cannot access kernel mappings

6. Kernel stacks per task
   - separate kernel stack for each process

7. Page fault handler
   - read CR2
   - decode error code
   - distinguish:
       * user invalid access
       * kernel bug
       * missing page
   - kill process or panic accordingly

8. Address space creation/destruction
   - create_user_pd()
   - destroy_pd()
   - free user mappings without touching kernel mappings

9. ELF loader integration
   - map PT_LOAD segments
   - respect PF_R/PF_W/PF_X
   - map stack
   - set initial instruction pointer

10. User pointer validation
    - validate ranges
    - check page permissions
    - prevent wrapping:
        addr + size < addr

11. TLB handling
    - invlpg after unmapping/remapping
    - CR3 reload when switching address spaces

12. Shared kernel mappings
    - clone kernel PDEs into new processes
    - keep kernel heap/direct mappings consistent

## Syscalls

1. openat
2. actually use cwd
3. execve
4. fork deep copy
5. waitpid
6. creat
7. chdir
8. stat
9. lseek
10. getpid
11. kill
12. rename
13. mkdir
14. rmdir
15. dup
