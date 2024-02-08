#pragma once
#include "shellclave.h"
int ptrace_attach(pid_t pid);
int ptrace_getregs(pid_t pid, USER_REGS_TYPE* regs);
enum {PRINT_SYSCALL_ONLY = 1, PRINT_ALL_REGS = 2};
int ptrace_getregs_print(pid_t pid, int options);
int ptrace_singlestep(pid_t pid, int signal);
int ptrace_cont(pid_t pid, int signal);
int ptrace_syscall(pid_t pid, int signal);

enum PTRACE_RET_STATUS
{
    PTRACE_STOP_UNKNOWN = 0,
    PTRACE_STOP_SYSCALL,
    PTRACE_STOP_SYSCALL_BEGIN,
    PTRACE_STOP_SYSCALL_END,
    PTRACE_STOP_SIGNAL,
    PTRACE_STOP_CLONE,
    PTRACE_STOP_EXECVE,
    PTRACE_STOP_EXIT,
    PTRACE_STOP_FORK,
    PTRACE_STOP_VFORK,
    PTRACE_STOP_VFORKDONE,
    PTRACE_STOP_SECCOMP,
    PTRACE_PROCESS_EXIT,
};
int ptrace_wait_for_process(pid_t pid);
int ptrace_get_stop_reason(int waitpid_status);