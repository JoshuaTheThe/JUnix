
#ifndef SIG_H
#define SIG_H

#include <stdint.h>

typedef enum
{
        SIG_NONE = 0,
        SIGHUP    = 1,   // Hangup
        SIGINT    = 2,   // Interrupt (Ctrl+C)
        SIGQUIT   = 3,   // Quit (Ctrl+\)
        SIGILL    = 4,   // Illegal instruction
        SIGTRAP   = 5,   // Breakpoint/trace trap
        SIGABRT   = 6,   // Abort
        SIGBUS    = 7,   // Bus error
        SIGFPE    = 8,   // Floating-point exception
        SIGKILL   = 9,   // Kill (cannot be caught or ignored)
        SIGUSR1   = 10,  // User-defined signal 1
        SIGSEGV   = 11,  // Segmentation fault
        SIGUSR2   = 12,  // User-defined signal 2
        SIGPIPE   = 13,  // Broken pipe
        SIGALRM   = 14,  // Alarm clock
        SIGTERM   = 15,  // Termination request
        SIGSTKFLT = 16,  // Stack fault (Linux-specific)
        SIGCHLD   = 17,  // Child stopped or exited
        SIGCONT   = 18,  // Continue if stopped
        SIGSTOP   = 19,  // Stop (cannot be caught or ignored)
        SIGTSTP   = 20,  // Terminal stop (Ctrl+Z)
        SIGTTIN   = 21,  // Background read from tty
        SIGTTOU   = 22,  // Background write to tty
        SIGURG    = 23,  // Urgent socket condition
        SIGXCPU   = 24,  // CPU time limit exceeded
        SIGXFSZ   = 25,  // File size limit exceeded
        SIGVTALRM = 26,  // Virtual timer expired
        SIGPROF   = 27,  // Profiling timer expired
        SIGWINCH  = 28,  // Window size changed
        SIGIO     = 29,  // I/O available
        SIGPWR    = 30,  // Power failure
        SIGSYS    = 31,  // Bad system call
        SIG_MAX   = 31
} sig_t;

typedef enum
{
        EOK             = 0,   // Success

        EPERM           = 1,   // Operation not permitted
        ENOENT          = 2,   // No such file or directory
        ESRCH           = 3,   // No such process
        EINTR           = 4,   // Interrupted system call
        EIO             = 5,   // I/O error
        ENXIO           = 6,   // No such device or address
        E2BIG           = 7,   // Argument list too long
        ENOEXEC         = 8,   // Exec format error
        EBADF           = 9,   // Bad file descriptor
        ECHILD          = 10,  // No child processes
        EAGAIN          = 11,  // Resource temporarily unavailable
        ENOMEM          = 12,  // Out of memory
        EACCES          = 13,  // Permission denied
        EFAULT          = 14,  // Bad address
        ENOTBLK         = 15,  // Block device required
        EBUSY           = 16,  // Device or resource busy
        EEXIST          = 17,  // File exists
        EXDEV           = 18,  // Cross-device link
        ENODEV          = 19,  // No such device
        ENOTDIR         = 20,  // Not a directory
        EISDIR          = 21,  // Is a directory
        EINVAL          = 22,  // Invalid argument
        ENFILE          = 23,  // Too many open files in system
        EMFILE          = 24,  // Too many open files
        ENOTTY          = 25,  // Inappropriate ioctl
        ETXTBSY         = 26,  // Text file busy
        EFBIG           = 27,  // File too large
        ENOSPC          = 28,  // No space left on device
        ESPIPE          = 29,  // Illegal seek
        EROFS           = 30,  // Read-only filesystem
        EMLINK          = 31,  // Too many links
        EPIPE           = 32,  // Broken pipe
        EDOM            = 33,  // Math argument out of domain
        ERANGE          = 34,  // Math result not representable

        ERRNO_MAX       = 34
} errno_t;

#endif
