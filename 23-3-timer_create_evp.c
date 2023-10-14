#define _POSIX_C_SOURCE 199309L

#include <signal.h>
#include <time.h>

void handler(int sig, siginfo_t *info, void *ucontext)
{
    printf("got signal %d, signal source is %d, alarm code is %d\n", sig, info->si_code, info->si_value.sival_int);
}

int main()
{
    timer_t tt;
    timer_create(CLOCK_REALTIME, NULL, &tt);
    return 0;
}