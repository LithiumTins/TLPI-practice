#define _XOPEN_SOURCE 500

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "tlpi_hdr.h"

int main()
{
    switch (vfork())
    {
    case -1:
        __exit();
        break;
    case 0:
        close(STDOUT_FILENO);
        break;
    default:
        printf("你好\n");
    }
}