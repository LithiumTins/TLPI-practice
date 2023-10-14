#include <sched.h>
#include <sys/times.h>
#include <stdlib.h>
#include "tlpi_hdr.h"

#define DIFF_4TIME(x, y) ((((x).tms_utime + (x).tms_stime) - ((y).tms_utime) + (y).tms_stime) * 4 / CLOCKS_PER_SEC)

void use_time(void)
{
    struct tms start, cur, last_yield, last_print;
    times(&start);
    last_yield.tms_utime = last_print.tms_utime = start.tms_utime;
    last_yield.tms_stime = last_print.tms_stime = start.tms_stime;

    while (1)
    {
        times(&cur);
        if (DIFF_4TIME(cur, start) >= 12) break;
        if (DIFF_4TIME(cur, last_print) >= 1)
        {
            last_print.tms_utime = cur.tms_utime;
            last_print.tms_stime = cur.tms_stime;
            printf("Pid: %ld, CPU time: %ld\n", (long)getpid(), (long)DIFF_4TIME(cur, start) / 4);
        }
        if (DIFF_4TIME(cur, last_yield) >= 4)
        {
            last_yield.tms_utime = cur.tms_utime;
            last_yield.tms_stime = cur.tms_stime;
            sched_yield();
        }
    }
}

int main(int argc, char *argv[])
{
    struct sched_param param;
    param.sched_priority = 30;
    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1)
        errExit("sched_setscheduler");

    if (fork() == -1)
        errExit("fork");
    
    use_time();

    return 0;
}