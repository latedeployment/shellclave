#include "shellclave.h"
#include "shellclave_ptrace_linux.h"
#include <elf.h> // this is for arm
#include <linux/uio.h> // this is for arm
#include <sys/syscall.h>


int ptrace_attach(pid_t pid)
{
    return ptrace(PTRACE_ATTACH, pid, NULL, NULL);
}

int ptrace_getregs_print(pid_t pid, int options)
{
    struct user_regs_struct regs;
    memset(&regs, 0xdd, sizeof(regs)); // just for fill garbage to detect later

    if (ptrace_getregs(pid, &regs))
    {
        return EINVAL;
    }

#if SHELLCLAVE_ARCH_X86
    // TBD
    printf("EIP/RIP: 0x%llx\n", regs.rip);
    // Print other x86/x86_64 registers...
#elif SHELLCLAVE_ARCH_ARM
    printf("\n=========\n");
    int regi = 0;
    switch (options)
    {
        case PRINT_ALL_REGS:

            printf("\n=========\n");
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; j++, regi++)
                {
                    printf("reg%d: 0x%llx \t", regi, regs.regs[regi]);
                }
                printf("\n");
            }
            printf("PC: 0x%llx\n", regs.pc);
            printf("SP: 0x%llx\n", regs.sp);

            break;
        case PRINT_SYSCALL_ONLY:
            printf("SYSCALL: %lld\n", regs.regs[8]);
            printf("args:\n");


            for (int i = 0; i < 2; ++i)
            {
                for (int j = 0; j < 4; j++, regi++)
                {
                    printf("arg%d: 0x%llx \t", regi, regs.regs[regi]);
                }
                printf("\n");
            }
            printf("PC: 0x%llx\n", regs.pc);
            printf("SP: 0x%llx\n", regs.sp);
            break;
    }
    printf("\n=========\n");

#endif
    return 0;
}

int ptrace_getregs(pid_t pid, USER_REGS_TYPE* regs)
{
    if (regs == NULL)
    {
        return EINVAL;
    }
    struct iovec iov =
    {
        .iov_base = regs,
        .iov_len = sizeof (struct user_regs_struct),
    };

    return ptrace(PTRACE_GETREGSET, pid, 1, &iov);
}

int ptrace_singlestep(pid_t pid, int signal)
{
    return ptrace(PTRACE_SYSEMU_SINGLESTEP, pid, NULL, signal);
}

int ptrace_cont(pid_t pid, int signal)
{
    return ptrace(PTRACE_CONT, pid, NULL, signal);
}

int ptrace_syscall(pid_t pid, int signal)
{
    return ptrace(PTRACE_SYSCALL, pid, NULL, signal);
}

int ptrace_get_stop_reason(int waitpid_status)
{
    if (WIFEXITED(waitpid_status))
    {
        return PTRACE_PROCESS_EXIT;
    }
    else if (WIFSIGNALED(waitpid_status))
    {
        return PTRACE_STOP_SIGNAL;
    }
    else if (WIFSTOPPED(waitpid_status))
    {
        int stop_signal = WSTOPSIG(waitpid_status);
        if (stop_signal == SIGTRAP)
        {
            // Check for different types of stops
            switch (waitpid_status >> 16)
            {
                case PTRACE_EVENT_STOP:
                    return PTRACE_STOP_UNKNOWN;
                case PTRACE_EVENT_SECCOMP:
                    return PTRACE_STOP_SECCOMP;
                case PTRACE_EVENT_EXEC:
                    return PTRACE_STOP_EXECVE;
                case PTRACE_EVENT_VFORK_DONE:
                    return PTRACE_STOP_VFORKDONE;
                case PTRACE_EVENT_FORK:
                    return PTRACE_STOP_FORK;
                case PTRACE_EVENT_CLONE:
                    return PTRACE_STOP_CLONE;
                default:
                    return PTRACE_STOP_UNKNOWN;
            }
        }
        // TBD handle syscall-exit-stop (seems quite hard)
        else if (stop_signal == (SIGTRAP | 0x80))
        {
            return PTRACE_STOP_SYSCALL;
        }
        return PTRACE_STOP_UNKNOWN;
    }
    else
    {
        // we don't know exactly what is going on
        // return -status so maybe the caller will know better
        // (probably not...)
        return -waitpid_status;
    }
}

// returns a waitpid stop reason >= 0 and errno as -errno
// check against >= 0 for success and < 0 for failure
int ptrace_wait_for_process(pid_t pid)
{
    int wait_status = 0;
    int waitpid_ret = waitpid(pid, &wait_status, __WALL);
    if (waitpid_ret > 0)
    {
        return ptrace_get_stop_reason(wait_status);
    }
    else
    {
        return -wait_status;
    }
}