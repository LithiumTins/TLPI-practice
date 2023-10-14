#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include "tlpi_hdr.h"

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};


int
main(int argc, char *argv[])
{
    int semid = semget(IPC_PRIVATE, 1, 0);
    if (semid == -1)
        errExit("semget");

    union semun ini_buf;
    ini_buf.val = 0;
    if (semctl(semid, 0, SETVAL, ini_buf) == -1)
        errExit("semctl");

    struct sembuf buf;

    switch (fork())
    {
    case -1:
        errExit("fork");
        break;
    case 0: // Child
        sleep(2);
        buf.sem_num = 0;
        buf.sem_op = +1;
        buf.sem_flg = 0;
        printf("Child finished\n");
        if (semop(semid, &buf, 1) == -1)
            errExit("semop");
        exit(EXIT_SUCCESS);
    default:
        printf("Waiting for child to do things\n");
        buf.sem_num = 0;
        buf.sem_op = -1;
        buf.sem_flg = 0;
        while (semop(semid, &buf, 1) == -1)
            if (errno != EINTR)
                errExit("semop");
        printf("Parent exit\n");
        exit(EXIT_SUCCESS);
    }

    return 0;
}