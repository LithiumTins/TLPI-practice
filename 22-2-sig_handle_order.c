#define _POSIX_C_SOURCE 1919810L
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

void handler(int sig)
{
    printf("Receive signal %d\n", sig);
}

int main()
{
    printf("The minimum realtime signal number is %d\n", SIGRTMIN);
    printf("The maximum realtime signal number is %d\n", SIGRTMAX);

    struct sigaction sa;
    sa.sa_flags = 0;
    sigfillset(&sa.sa_mask);
    sa.sa_handler = handler;

    sigset_t origin, block;
    sigfillset(&block);
    sigprocmask(SIG_BLOCK, &block, &origin);
    printf("Now all signals are blocked\n");

    for (int i = 1; i <= SIGRTMAX; i++)
        sigaction(i, &sa, NULL);
    printf("Now the handler is set for all signals\n");
    printf("Then the program will wait for 60 seconds\n");

    sleep(60);

    printf("Then all signals will be unblocked\n");
    sigprocmask(SIG_SETMASK, &origin, NULL);

    while (1) ;

    return 0;
}