#include "shellclave.h"
#include "shellclave_cli.h"
#include "shellclave_ptrace_linux.h"


int attach_to_process(pid_t slave_pid)
{
    int ret = ptrace_attach(slave_pid);

    if (ret < 0)
    {
        switch (errno)
        {
            case ESRCH:
                DUMP_ERROR("no such process %d", slave_pid);
                break;
            default:

        }
        return errno;
    }

    int wait_status = ptrace_wait_for_process(slave_pid);

    if (wait_status >= 0)
    {
        DUMP_INFO("attached to %d\n", slave_pid);
        return 0;
    }
    else
    {
        return wait_status;
    }
}

pid_t run_debug_process()
{
    const char* slave_example = DEBUG_PROCESS_NAME;
    char* const args[] = {DEBUG_PROCESS_NAME, "5", NULL};
    pid_t debug_child = create_child_process(slave_example, args, NULL, NULL);
    return debug_child;
}

int shellclave_main(struct cmd_options* cmd_options)
{
    if (cmd_options == NULL)
    {
        return EINVAL;
    }

    if (cmd_options->run_debug_process)
    {
        cmd_options->slave_pid = run_debug_process();
        if (cmd_options->slave_pid < 1)
        {
            DUMP_ERROR("failed to create debug process");
            return ECHILD;
        }
    }

    printf("slave PID: %d (trying to attach...)\n", cmd_options->slave_pid);

    int attach_status = attach_to_process(cmd_options->slave_pid);
    if (attach_status)
    {
        DUMP_ERROR("failed to attach to %d (errno: %d)\n", cmd_options->slave_pid,
                   attach_status);
        return attach_status;
    }

    if (cmd_options->debug_cli)
    {
        return start_debug_cli(cmd_options->slave_pid);
    }

    return 0;
}

int shellclave_terminate(struct cmd_options* cmd_options)
{
    if (cmd_options->run_debug_process &&
            cmd_options->slave_pid > 0)
    {
        DUMP_INFO("killing debug process %d", cmd_options->slave_pid);
        kill(cmd_options->slave_pid, SIGKILL);
    }
    return 0;
}

