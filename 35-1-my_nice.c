#include <unistd.h>
#include <sys/resource.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    int prior = getpriority(PRIO_PROCESS, 0);
    
    if (strcmp(argv[1], "-n") == 0)
        prior += atoi(argv[2]);
    else
        prior = atoi(argv[2] + 1);
    
    setpriority(PRIO_PROCESS, 0, prior);

    execv(argv[3], argv + 3);

    return 0;
}