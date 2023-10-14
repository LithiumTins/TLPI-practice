#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    mode_t perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    int fd1 = open("ft", O_WRONLY | O_CREAT | O_TRUNC, perms);
    if (fd1 == -1) errExit("open");

    if (write(fd1, "123456", 6) != 6) errExit("write");

    int fd2 = dup(fd1);
    if (write(fd2, "7", 1) != 1) errExit("write");

    int fd3 = open("ft", O_WRONLY | O_APPEND);

    int fd4 = dup(fd3);
    if (lseek(fd4, 0, SEEK_SET) == -1) errExit("lseek");
    if (write(fd4, "8", 1) != 1) errExit("write");

    close(fd1);
    close(fd2);
    close(fd3);
    close(fd4);

    int fd5 = open("ft", O_RDONLY);
    char buf[11] = {};
    if (read(fd5, buf, 10) < 0) errExit("read");
    close(fd5);

    printf("%s", buf);

    return 0;
}