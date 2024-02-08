#include "shellclave_cli.h"

#include <readline/readline.h>
#include <readline/history.h>
#include "shellclave_ptrace_linux.h"

// not everything is implemented
#define CMD_PEEKTEXT "peektext"
#define CMD_PEEKUSER "peekuser"
#define CMD_POKETEXT "poketext"
#define CMD_POKEUSER "pokeuser"
#define CMD_GETREGS "getregs"
#define CMD_POKEREGS "pokeregs"
#define CMD_GETSIGINFO "getsiginfo"
#define CMD_SETSIGINFO "setsiginfo"
#define CMD_PEEKSIGINFO "peeksiginfo"
#define CMD_CONT "cont"
#define CMD_SYSCALL "syscall"
#define CMD_SINGLESTEP "singlestep"
#define CMD_KILL "kill"
#define CMD_INTERRUPT "interrupt"
#define CMD_DETACH "detach"
#define CMD_EXIT "exit"
#define CMD_HELP "help"
#define CMD_FOLLOW "follow"


char* cli_options[] =
{
    CMD_PEEKTEXT,
    CMD_PEEKUSER,
    CMD_POKETEXT,
    CMD_POKEUSER,
    CMD_GETREGS,
    CMD_POKEREGS,
    CMD_GETSIGINFO,
    CMD_SETSIGINFO,
    CMD_PEEKSIGINFO,
    CMD_CONT,
    CMD_SYSCALL,
    CMD_SINGLESTEP,
    CMD_KILL,
    CMD_INTERRUPT,
    CMD_DETACH,
    CMD_EXIT,
    CMD_FOLLOW,
    NULL
};

char* completion_generator(const char* text, int state)
{
    static int list_index, len;
    char* name;

    if (!state)
    {
        list_index = 0;
        len = strlen(text);
    }

    while ((name = cli_options[list_index]))
    {
        list_index++;

        if (strncmp(name, text, len) == 0)
        {
            return strdup(name);
        }
    }

    return NULL;
}

char** completion(const char* text, int start, int end)
{
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, completion_generator);
}

int32_t start_debug_cli(pid_t slave)
{
    int should_terminate = 0;
    rl_attempted_completion_function = completion;

    while (!should_terminate)
    {
        const char* user_request = readline( "shellclave> " );

        if (!strcmp(user_request, CMD_EXIT))
        {
            should_terminate = 1;
        }
        else if (!strcmp(user_request, CMD_HELP))
        {
            printf("TBD!\n");
        }
        else if (!strcmp(user_request, CMD_GETREGS))
        {
            ptrace_getregs_print(slave, PRINT_ALL_REGS);
        }
        else if (!strcmp(user_request, CMD_SINGLESTEP))
        {
            ptrace_singlestep(slave, 0);
        }
        else if (!strcmp(user_request, CMD_CONT))
        {
            ptrace_cont(slave, 0);
        }
        else if (!strcmp(user_request, CMD_SYSCALL))
        {
            ptrace_syscall(slave, 0);
        }
        else if (!strcmp(user_request, CMD_FOLLOW))
        {
            while (1)
            {
                ptrace_getregs_print(slave, PRINT_SYSCALL_ONLY);
                ptrace_syscall(slave, 0);
                ptrace_wait_for_process(slave);
            }
        }

    };

    return 0;
}