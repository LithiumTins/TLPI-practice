#define _XOPEN_SOURCE 500

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>

struct bin_sem
{
    int fifo;
    char file[15];
};

int initAvail(struct bin_sem *sem)
{
    strcpy(sem->file, "/tmp/semXXXXXX");
    sprintf(sem->file, "%d", time(NULL) % 1000000);

    sem->fifo = mkfifo(sem->file, S_IWUSR | S_IRUSR);
    if (sem->fifo == -1)
        return -1;
    if (write(sem->fifo, "x", 1) == -1)
        return -1;

    return 0;
}

int initUsed(struct bin_sem *sem)
{
    strcpy(sem->file, "/tmp/semXXXXXX");
    sprintf(sem->file, "%d", time(NULL) % 1000000);

    sem->fifo = mkfifo(sem->file, S_IWUSR | S_IRUSR);
    if (sem->fifo == -1)
        return -1;

    return 0;
}

int reserveSem(struct bin_sem *sem)
{
    char buf;
    while (read(sem->fifo, &buf, 1) != 1)
        if (errno != EINTR)
            return -1;

    return 0;
}

int releaseSem(struct bin_sem *sem)
{
    if (write(sem->fifo, "x", 1) == -1)
        return -1;
    
    return 0;
}