#include <unistd.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    if (setpgid(0, 0) == -1)
        errExit("setpgid");

    if (setsid() == -1)
        printf("Can't set sid\n");
    else printf("Sid set\n");

    return 0;
}