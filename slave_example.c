#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <minutes>\n", argv[0]);
        return 1;
    }

    int minutes = atoi(argv[1]);
    if (minutes <= 0)
    {
        fprintf(stderr, "Invalid value for minutes.\n");
        return 1;
    }

    int seconds = minutes * 60;
    pid_t pid = getpid();

    printf("Running for %d minutes... in %d PID\n", minutes, pid);

    for (int i = 0; i < seconds * 10; ++i)
    {
        printf("Still alive - %d seconds elapsed.\n", i);
        sleep(10); // Sleep for 60 seconds (1 minute)
    }

    printf("Finished after %d minutes.\n", minutes);

    return 0;
}