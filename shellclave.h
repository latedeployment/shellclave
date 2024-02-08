#pragma once
#include "shellclave_config.h"

#ifdef __linux__
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <syscall.h>
#endif
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <spawn.h>
#include <fcntl.h>
#include <seccomp.h>

#if defined(__i386__) || defined(__x86_64__)
#define SHELLCLAVE_ARCH_X86 1
#define SHELLCLAVE_ARCH_ARM 0
#define USER_REGS_TYPE struct user_regs_struct
#elif defined(__arm__) || defined (__ARM_ARCH)
#define SHELLCLAVE_ARCH_ARM 1
#define SHELLCLAVE_ARCH_X86 0
#define USER_REGS_TYPE struct user_regs_struct
#else
#error "Unsupported architecture"
#endif


#define DUMP_ERROR(fmt, ...) \
    fprintf(stderr, "shellclave: error: " fmt "\n", ##__VA_ARGS__)

#define DUMP_INFO(fmt, ...) \
    fprintf(stdout, "shellclave: " fmt "\n", ##__VA_ARGS__)


struct cmd_options
{
    pid_t slave_pid;
    u_int32_t run_debug_process:1, debug_cli: 1, _reserved:30;
};



int create_child_process(const char* executable, char* const argv[],
                         int* stdout_pipe, int* stderr_pipe);


int create_child_process_with_filters(const char* executable,
                                      char* const argv[],
                                      int* stdout_pipe, int* stderr_pipe,
                                      const scmp_filter_ctx seccomp_ctx);

int shellclave_main(struct cmd_options* cmd_options);
int shellclave_terminate(struct cmd_options* cmd_options);


