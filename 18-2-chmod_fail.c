#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    int fd;

    mkdir("test", S_IRUSR | S_IWUSR | S_IXUSR);
    chdir("test");
    fd = open("myfile", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    symlink("myfile", "../mylink");
    errno = 0;
    chmod("../mylink", S_IRUSR);
    errExit("chmod");

    return 0;
}