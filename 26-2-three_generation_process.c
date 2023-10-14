#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "tlpi_hdr.h"

int main()
{
    switch (fork())
    {
    case -1:
        errExit("fork");
        break;
    case 0:
        switch (fork())
        {
        case -1:
            errExit("fork");
            break;
        case 0:
            printf("PPid is %ld\n", (long)getppid());
            sleep(10);
            printf("PPid is %ld\n", (long)getppid());
            sleep(10);
            printf("PPid is %ld\n", (long)getppid());
            break;
        default:
            sleep(5);
            printf("Now parent process turns into a zombie process\n");
            exit(EXIT_SUCCESS);
            break;
        }
        break;
    default:
        sleep(15);
        if (wait(NULL) == -1)
            errExit("wait");
        printf("Now the parent process is recycled\n");
        break;
    }

    return 0;
}