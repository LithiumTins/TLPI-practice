#define _XOPEN_SOURCE 500

#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    if (argc != 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s source dest\n", argv[0]);

    int rfd = open(argv[1], O_RDONLY);
    if (rfd == -1)
        errExit("open - r");

    int wfd = open(argv[2], O_CREAT | O_RDWR);
    if (wfd == -1)
        errExit("open - w");

    struct stat st;
    if (fstat(rfd, &st) == -1)
        errExit("fstat");
    
    if (ftruncate(wfd, st.st_size) == -1)
        errExit("ftruncate");

    void *source, *dest;

    source = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, rfd, 0);
    if (source == -1)
        errExit("mmap - source");

    dest = mmap(NULL, st.st_size, PROT_WRITE, MAP_SHARED, wfd, 0);
    if (dest == -1)
        errExit("mmap - dest");

    memcpy(dest, source, st.st_size);

    return 0;
}