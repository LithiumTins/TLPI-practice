#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int fd = open("1.txt", O_WRONLY | O_APPEND);
    if (fd == -1) return -1;

    int p = lseek(fd, 0, SEEK_SET);
    if (p == -1) return -1;

    int writeNum = write(fd, "hh", 2);
    if (writeNum == -1) return -1;

    return 0;
}