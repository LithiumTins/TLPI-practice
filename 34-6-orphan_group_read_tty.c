#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    char buf[100];
    switch(fork())
    {
    case -1:
        errExit("fork");
    case 0:
        sleep(5);
        if (read(STDIN_FILENO, buf, 100) == -1 && errno == EIO)
            printf("We got EIO\n");
    default:
        exit(EXIT_SUCCESS);
    }
}