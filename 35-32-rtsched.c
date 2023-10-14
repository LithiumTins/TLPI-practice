#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    int policy = (argv[1][0] == 'r') ? SCHED_RR : SCHED_FIFO;
    int prior = atoi(argv[2]);
    struct sched_param param;

    param.sched_priority = prior;
    if (sched_setscheduler(0, policy, &param) == -1)
        errExit("sched_setscheduler");

    if (setreuid(getuid(), getuid()) == -1)
        errExit("setreuid");
    
    execv(argv[3], argv + 3);
    
    return -1;
}