#define _GNU_SOURCE
#include <unistd.h>
#include <signal.h>
#include "tlpi_hdr.h"

void handler(int sig)
{
    printf("got signal %d(%s)\n", sig, strsignal(sig));
}

int main(int argc, char *argv[])
{
    pid_t child;

    switch(child = fork())
    {
    case -1:
        errExit("fork");
    case 0:
        if (signal(SIGTTIN, handler) == -1)
            errExit("signal");
        pause();
        printf("Child\n");
    default:
        printf("Parent exit(Child's pid is %ld)\n", (long)child);
        exit(EXIT_SUCCESS);
    }

    return 0;
}