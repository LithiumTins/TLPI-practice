#include <sys/times.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/wait.h>
#include "tlpi_hdr.h"

void
printRusage(const char *leader, const struct rusage *ru)
{
    const char *ldr = (leader == NULL) ? "" : leader;

    printf("%sCPU time (secs):         user=%.3f; system=%.3f\n", ldr,
            ru->ru_utime.tv_sec + ru->ru_utime.tv_usec / 1000000.0,
            ru->ru_stime.tv_sec + ru->ru_stime.tv_usec / 1000000.0);
    printf("%sMax resident set size:   %ld\n", ldr, ru->ru_maxrss);
    printf("%sIntegral shared memory:  %ld\n", ldr, ru->ru_ixrss);
    printf("%sIntegral unshared data:  %ld\n", ldr, ru->ru_idrss);
    printf("%sIntegral unshared stack: %ld\n", ldr, ru->ru_isrss);
    printf("%sPage reclaims:           %ld\n", ldr, ru->ru_minflt);
    printf("%sPage faults:             %ld\n", ldr, ru->ru_majflt);
    printf("%sSwaps:                   %ld\n", ldr, ru->ru_nswap);
    printf("%sBlock I/Os:              input=%ld; output=%ld\n",
            ldr, ru->ru_inblock, ru->ru_oublock);
    printf("%sSignals received:        %ld\n", ldr, ru->ru_nsignals);
    printf("%sIPC messages:            sent=%ld; received=%ld\n",
            ldr, ru->ru_msgsnd, ru->ru_msgrcv);
    printf("%sContext switches:        voluntary=%ld; "
            "involuntary=%ld\n", ldr, ru->ru_nvcsw, ru->ru_nivcsw);
}

int main(int argc, char *argv[])
{
    struct rusage usage;
    switch (fork())
    {
    case -1:
        errExit("fork");
        break;
    case 0:
        for (int i = 0; i < 1000000; i++) ;
        printf("child finished\n");
        break;
    default:
        if (getrusage(RUSAGE_CHILDREN, &usage) == -1)
            errExit("getrusage");
        printRusage("Before child end: ", &usage);
        sleep(5);
        if (getrusage(RUSAGE_CHILDREN, &usage) == -1)
            errExit("getrusage");
        printRusage("Before wait: ", &usage);
        int status;
        if (wait(&status) == -1)
            errExit("wait");
        if (getrusage(RUSAGE_CHILDREN, &usage) == -1)
            errExit("getrusage");
        printRusage("After wait: ", &usage);
        break;
    }

    return 0;
}