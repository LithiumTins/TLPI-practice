#include <signal.h>
#include <unistd.h>
#include "tlpi_hdr.h"

static void
handler(int sig)
{
    printf("handle begin\n");
    sleep(5);
    printf("handle end\n");
}

int main(int argc, char *argv[])
{
    struct sigaction sg;
    sg.sa_handler = handler;
    sigemptyset(&sg.sa_mask);
    sg.sa_flags = SA_NODEFER;
    if (sigaction(SIGINT, &sg, NULL) == -1)
        errExit("sigaction");

    return 0;
}