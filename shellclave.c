#include "shellclave_cli.h"

// TODO: capture ctrl-c and terminate process and child processes created


enum { PID_OPTION = 'p', DEBUG_CLI_OPTION='c', DEBUG_OPTION = 'd', DUMMY_PID = -2};

static struct argp_option options[] =
{
    {"pid", PID_OPTION, "PID", 0, "slave PID"},
    {"debug", DEBUG_OPTION, 0, 0, "run debug process"},
    {"cli", DEBUG_CLI_OPTION, 0, 0, "enable CLI"},
    {0}
};

static error_t parse_opt(int key, char* arg, struct argp_state* state)
{
    struct cmd_options* cmd_options = state->input;

    switch (key)
    {
        case PID_OPTION:
            cmd_options->slave_pid = atoi(arg);
            break;
        case ARGP_KEY_ARG:
            argp_usage(state);
            break;
        case DEBUG_OPTION:
            cmd_options->run_debug_process = 1;
            cmd_options->slave_pid = DUMMY_PID; // dummy PID - will get a new one later
            break;
        case DEBUG_CLI_OPTION:
            cmd_options->debug_cli = 1;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

// Argp parser
static struct argp argp = { options, parse_opt, 0, 0 };

int handle_cmdline_args(int argc, char** argv, struct cmd_options* cmd_options)
{
    if (cmd_options == NULL)
    {
        return EINVAL;
    }

    // initialize stuff
    memset(cmd_options, 0x0, sizeof(struct cmd_options));

    cmd_options->slave_pid = -1;

    error_t ret = argp_parse(&argp, argc, argv, 0, 0, cmd_options);
    if (ret)
    {
        return ret;
    }

    // check required arguments
    if (cmd_options->slave_pid == -1)
    {
        DUMP_ERROR("slave PID required --pid or -p");
        return EINVAL;
    }

    return 0;
}

int main(int argc, char** argv)
{
    struct cmd_options cmd_options;

    if (handle_cmdline_args(argc, argv, &cmd_options))
    {
        return EINVAL;
    }

    // start annoying the proces
    int ret = shellclave_main(&cmd_options);

    // this has to happen anyway
    shellclave_terminate(&cmd_options);
    return ret;
}
