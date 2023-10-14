#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "tlpi_hdr.h"

int mydup(int oldfd)
{
    return fcntl(oldfd, F_DUPFD, 0);
}

int mydup2(int oldfd, int newfd)
{
    if (newfd < 0) return -1;

    int check = fcntl(oldfd, F_GETFL);
    if (check == -1)
    {
        errno = EBADF;
        return -1;
    }
    if (oldfd == newfd) return newfd;

    check = fcntl(newfd, F_GETFL);
    if (check != -1) close(newfd);

    return fcntl(oldfd, F_DUPFD, newfd);
}