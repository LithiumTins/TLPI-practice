#define _XOPEN_SOURCE 500
#include <signal.h>
#include <stdio.h>

void (*sigset(int sig, void (*handler)(int)))(int)
{
    struct sigaction sa, origin;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;

    sigset_t block, mask;
    sigemptyset(&block);
    sigaddset(&block, sig);

    if (handler == SIG_HOLD)
        sigprocmask(SIG_BLOCK, &block, &mask);
    else
        sigprocmask(SIG_BLOCK, NULL, &mask);

    if (handler != SIG_HOLD)
        if (sigaction(sig, &sa, &origin) == -1)
            return SIG_ERR;

    if (sigismember(&mask, sig))
        return SIG_HOLD;
    else
        return origin.sa_handler;
}

int sighold(int sig)
{
    sigset_t block;
    sigemptyset(&block);
    return sigprocmask(SIG_BLOCK, &block, NULL);
}

int sigrelse(int sig)
{
    sigset_t release;
    sigemptyset(&release);
    return sigprocmask(SIG_UNBLOCK, &release, NULL);
}

int sigignore(int sig)
{
    struct sigaction sa;
    sa.sa_flags = 0;
    sigfillset(&sa.sa_mask);
    sa.sa_handler = SIG_IGN;
    return sigaction(sig, &sa, NULL);
}

int sigpause(int sig)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, sig);
    return sigsuspend(&mask);
}

int main()
{
    return 0;
}