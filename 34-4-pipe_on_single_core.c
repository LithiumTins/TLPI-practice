#define _GNU_SOURCE
#include <sched.h>
#include <stdlib.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(atoi(argv[1]), &cpuset);

    if (sched_setaffinity(0, sizeof(cpuset), &cpuset) == -1)
        errExit("sched_setaffinity");

    if (isatty(STDIN_FILENO))
    {
        for (int i = 0; i < 10000000; i++)
            write(STDOUT_FILENO, "h", 1);
    }
    else
    {
        char buf;
        for (int i = 0; i < 10000000; i++)
            read(STDIN_FILENO, &buf, 1);
        printf("over\n");
    }

    return 0;
}