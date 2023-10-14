#include <signal.h>
#include <stdio.h>
#include <unistd.h>

static void handler(int sig)
{
    printf("hh\n");
}

int main(int argc, char *argv[])
{
    struct sigaction sg;
    sg.sa_handler = handler;
    sigemptyset(&sg.sa_mask);
    sg.sa_flags = 0;
    sigaction(SIGINT, &sg, NULL);

    sigset_t mask, origin;
    sigfillset(&mask);
    sigprocmask(SIG_SETMASK, &mask, &origin);
    printf("Now we block all the signals for 5 seconds\n");
    sleep(5);
    sigprocmask(SIG_SETMASK, &origin, NULL);
    printf("Now we unlock the signals and you should get an hh\n");
    sigprocmask(SIG_SETMASK, &mask, NULL);
    printf("And we block signals for 5 seconds again\n");
    sleep(5);
    sg.sa_handler = SIG_IGN;
    sigaction(SIGINT, &sg, NULL);
    sigprocmask(SIG_SETMASK, &origin, NULL);
    printf("We set SIG_IGN for SIGINT and you won't get another hh");

    return 0;
}