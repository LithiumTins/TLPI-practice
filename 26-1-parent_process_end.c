#include <sys/types.h>
#include <unistd.h>
#include "tlpi_hdr.h"

int main()
{
    switch (fork())
    {
    case -1:
        errExit("fork");
        break;
    case 0:
        sleep(10);
        printf("Parent is %ld", (long)getppid());
        break;
    default:
        exit(EXIT_SUCCESS);
        break;
    }

    return 0;
}