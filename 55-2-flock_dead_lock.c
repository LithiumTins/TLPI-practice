#include <sys/file.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    int fd1, fd2;
    sem_t *sem;

    fd1 = open("f1", O_RDWR);
    if (fd1 == -1)
        errExit("open");
    
    fd2 = open("f2", O_RDWR);
    if (fd1 == -1)
        errExit("open");

    if ((sem = sem_open("/my_sem", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED)
        errExit("sem_open");
    if (sem_unlink("/my_sem") == -1)
        errExit("sem_unlink");

    switch (fork())
    {
    case -1:
        errExit("fork");
        break;
    case 0:
        if (flock(fd1, LOCK_EX) == -1)
            errExit("flock - child - first");
        if (sem_wait(sem) == -1)
            errExit("sem_wait");
        printf("child: wake\n");
        printf("child: post and try to get flock\n");
        sem_post(sem);
        if (flock(fd2, LOCK_EX) == -1)
            errExit("flock - child - second");
        pause();
        break;
    default:
        if (flock(fd2, LOCK_EX) == -1)
            errExit("flock - parent - first");
        if (sem_wait(sem) == -1)
            errExit("sem_wait");
        printf("parent: wake\n");
        printf("parent: post and try to get flock\n");
        sem_post(sem);
        if (flock(fd1, LOCK_EX) == -1)
            errExit("flock - parent - second");
        pause();
        break;
    }

    return 0;
}