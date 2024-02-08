#include "shellclave.h"

int create_child_process(const char* executable, char* const argv[],
                         int* stdout_pipe, int* stderr_pipe)
{
    pid_t pid;
    posix_spawn_file_actions_t actions;

    if (posix_spawn_file_actions_init(&actions) != 0)
    {
        DUMP_ERROR("failed to posix_spawn_file_actions_init, errno %d (%s)", errno,
                   strerror(errno));
        return -1;
    }

    // Set up redirection of stdout and stderr to pipes
    if (stdout_pipe)
    {
        posix_spawn_file_actions_adddup2(&actions, stdout_pipe[1], STDOUT_FILENO);
        close(stdout_pipe[1]);
    }
    if (stderr_pipe)
    {
        posix_spawn_file_actions_adddup2(&actions, stderr_pipe[1], STDERR_FILENO);
        close(stderr_pipe[1]);
    }

    int result = posix_spawn(&pid, executable, &actions, NULL, argv, NULL);

    posix_spawn_file_actions_destroy(&actions);

    if (result != 0)
    {
        DUMP_ERROR("posix_spawn failed: %d (%s)", result, strerror(result));
        return -1;
    }

    return pid;
}

int create_child_process_with_filters(const char* executable,
                                      char* const argv[],
                                      int* stdout_pipe, int* stderr_pipe,
                                      const scmp_filter_ctx seccomp_ctx)
{
    pid_t child_pid;
    if (!executable)
    {
        return -EINVAL;
    }

    // fork a child process (which will obtain the seccomp filter)

    child_pid = fork();

    if (child_pid == -1)
    {
        return -errno;
    }

    // if we are the child
    if (child_pid == 0)
    {
        // apply seccomp filters
        if (seccomp_ctx)
        {
            int seccomp_ret = seccomp_load(seccomp_ctx);

            if (seccomp_ret != 0)
            {
                DUMP_ERROR("seccomp_load failed: %d (%s)", seccomp_ret, strerror(seccomp_ret));
                exit(EXIT_FAILURE);
            }
        }

        // handle pipes
        if (stdout_pipe != NULL)
        {
            if (dup2(stdout_pipe[1], STDOUT_FILENO) == -1)
            {
                exit(EXIT_FAILURE);
            }
            close(stdout_pipe[0]);
            close(stdout_pipe[1]);
        }

        if (stderr_pipe != NULL)
        {
            if (dup2(stderr_pipe[1], STDERR_FILENO) == -1)
            {
                exit(EXIT_FAILURE);
            }
            close(stderr_pipe[0]);
            close(stderr_pipe[1]);
        }

        // execve the actual process
        execvp(executable, argv);

        // if we get here somehow
        exit(EXIT_FAILURE);
    }
    else
    {
        // Close the write ends of the pipes in the parent process
        if (stdout_pipe != NULL)
        {
            close(stdout_pipe[1]);
        }

        if (stderr_pipe != NULL)
        {
            close(stderr_pipe[1]);
        }

        return child_pid;
    }
}

// int example()
// {
//     int stdout_pipe[2], stderr_pipe[2];

//     if (pipe(stdout_pipe) == -1 || pipe(stderr_pipe) == -1)
//     {
//         perror("pipe");
//         return 1;
//     }

//     const char *executable = "/path/to/your/executable";
//     char *const args[] = {"/path/to/your/executable", "arg1", "arg2", NULL};

//     int pid = create_child_process(executable, args, stdout_pipe, stderr_pipe);
//     if (pid == -1)
//     {
//         perror("create_child_process");
//         return 1;
//     }

//     close(stdout_pipe[1]);
//     close(stderr_pipe[1]);

//     // Read and handle child process output from stdout_pipe[0] and stderr_pipe[0]

//     int status;
//     waitpid(pid, &status, 0);

//     return 0;
// }