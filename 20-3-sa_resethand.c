#include <signal.h>
#include "tlpi_hdr.h"

static void
handler(int sig)
{
    printf("next time will terminate the process\n");
}

int main(int argc, char *argv[])
{
    struct sigaction sg;
    sg.sa_handler = handler;
    sigfillset(&sg.sa_mask);
    sg.sa_flags = SA_RESETHAND;
    if (sigaction(SIGINT, &sg, NULL) == -1)
        errExit("sigaction");

    return 0;
}