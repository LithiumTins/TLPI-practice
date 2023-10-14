#include <sys/time.h>
#include <unistd.h>

unsigned int my_alarm(unsigned int seconds)
{
    struct itimerval itv, former;
    itv.it_value.tv_sec = seconds;
    itv.it_value.tv_usec = 0;
    itv.it_interval.tv_sec = 0;
    itv.it_interval.tv_sec = 0;
    setitimer(ITIMER_REAL, &itv, &former);
    return former.it_value.tv_sec + (former.it_value.tv_usec + 999999) / 1000000;
}