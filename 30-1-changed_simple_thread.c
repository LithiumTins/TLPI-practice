#include <pthread.h>
#include <stdio.h>
#include "tlpi_hdr.h"

struct thrd 
{
    int symbol;
    int loops;
};

static volatile int glob = 0;   /* "volatile" prevents compiler optimizations
                                   of arithmetic operations on 'glob' */
static void *                   /* Loop 'arg' times incrementing 'glob' */
threadFunc(void *arg)
{
    struct thrd *thr = (struct thrd *)arg;
    int loops = thr->loops;
    int loc, j;

    for (j = 0; j < loops; j++) {
        loc = glob;
        loc++;
        glob = loc;
        printf("Thread %d: glob is %d", thr->symbol, glob);
    }

    return NULL;
}

int
main(int argc, char *argv[])
{
    pthread_t t1, t2;
    int loops, s;
    struct thrd th1, th2;

    loops = (argc > 1) ? getInt(argv[1], GN_GT_0, "num-loops") : 10000000;
    th1.loops = th2.loops = loops;
    th1.symbol = 1;
    th2.symbol = 2;

    s = pthread_create(&t1, NULL, threadFunc, &th1);
    if (s != 0)
        errExitEN(s, "pthread_create");
    s = pthread_create(&t2, NULL, threadFunc, &th2);
    if (s != 0)
        errExitEN(s, "pthread_create");

    s = pthread_join(t1, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");
    s = pthread_join(t2, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");

    printf("glob = %d\n", glob);
    exit(EXIT_SUCCESS);
}