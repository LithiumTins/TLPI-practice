#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "tlpi_hdr.h"

struct iovec
{
    void * iov_base;
    size_t iov_len;
};

ssize_t my_readv(int fd, const struct iovec *iov, int iovcnt)
{
    ssize_t total = 0;
    ssize_t total_space = 0;

    for (int i = 0; i < iovcnt; i++)
        total_space += iov[i].iov_len;

    char * buffer = (char *)malloc(total_space);
    total = read(fd, buffer, total_space);
    if (total <= 0) return total; 

    int ind = 0;
    for (int i = 0; i < iovcnt && ind < total; i++)
    {
        int size = (total - ind) > iov[i].iov_len ? iov[i].iov_len : total - ind;
        memcpy(iov[i].iov_base, buffer + ind, size);
        ind += size;
    }

    free(buffer);
    return total;
}

ssize_t my_writev(int fd, const struct iovec *iov, int iovcnt)
{
    ssize_t total = 0;
    ssize_t total_space = 0;

    for (int i = 0; i < iovcnt; i++)
        total_space += iov[i].iov_len;
    
    char * buffer = (char *)malloc(total_space);
    int cnt = 0;
    for (int i = 0; i < iovcnt; i++)
    {
        memcpy(buffer + cnt, iov[i].iov_base, iov[i].iov_len);
        cnt += iov[i].iov_len;
    }

    total = write(fd, buffer, total_space);
    
    free(buffer);
    return total;
}

int main(int argc, char *argv[])
{
    return 0;
}