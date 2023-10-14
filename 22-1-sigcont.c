#define _POSIX_C_SOURCE 1919810L
#include <signal.h>
#include <unistd.h>
#include <time.h>

int tag = 10086;

void handler(int sig)
{
    tag = 0;
}

int main()
{
    int cnt = 0;

    sigset_t origin, block;
    sigemptyset(&block);
    sigaddset(&block, SIGCONT);

    sigprocmask(SIG_BLOCK, &block, &origin);

    struct sigaction sa;
    sa.sa_flags = 0;
    sigfillset(&sa.sa_mask);
    sa.sa_handler = handler;

    sigaction(SIGCONT, &sa, NULL);

    while (1)
    {
        sleep(1);
        cnt++;
        time_t tt = time(NULL);
        printf("tag is %d, time is %s\n", tag, ctime(&tt));
        if (cnt == 10) sigprocmask(SIG_SETMASK, &origin, &block);
    }

    return 0;
}