#define _POSIX_C_SOURCE 1919810L

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include "tlpi_hdr.h"

void handler(int sig)
{
    printf("hello\n");
}

int main()
{
    sigset_t new_mask, origin_mask;
    struct sigaction sa;
    int status;

    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &new_mask, &origin_mask);

    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    sigaction(SIGCHLD, &sa, NULL);

    switch (fork())
    {
    case -1:
        errExit("fork");
        break;
    case 0:
        _exit(EXIT_SUCCESS);
    default:
        break;
    }

    if (wait(&status) == -1)
        errExit("wait");

    sigprocmask(SIG_SETMASK, &origin_mask, NULL);

    return 0;
}