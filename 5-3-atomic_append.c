#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    if (argc < 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file num-bytes [x]\n", argv[0]);

    int fd;
    int flags = O_WRONLY | O_CREAT;
    mode_t perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    if (argc == 4 && strcmp(argv[3], "x") == 0)
        flags |= O_APPEND;

    fd = open(argv[1], flags, perms);
    if (fd == -1)
        errExit("open");
    
    int numBytes = getInt(argv[2], GN_ANY_BASE, "num-bytes");

    for (int i = 0; i < numBytes; i++)
    {
        if (argc == 4)
            if (lseek(fd, 0, SEEK_END) == -1)
                fatal("lseek failed");
        if (write(fd, "x", 1) != 1)
            fatal("write failed");
    }

    close(fd);

    return 0;
}