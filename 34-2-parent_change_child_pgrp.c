#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    pid_t child;

    if (argc != 1)
    {
        printf("exec\n");
        pause();
        exit(EXIT_SUCCESS);
    }

    switch (child = fork())
    {
    case -1:
        exit(EXIT_FAILURE);
        break;
    case 0:
        sleep(10);
        execl(argv[0], argv[0], 1, NULL);
        exit(EXIT_FAILURE);
    default:
        if (setpgid(child, 0) != -1) printf("Change the process group of child succesfully before exec\n");
        sleep(20);
        if (setpgid(child, getpgrp()) == -1) printf("Can't change the process group of child after exec\n");
        break;
    }
}