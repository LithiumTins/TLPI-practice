#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include "tlpi_hdr.h"

int main()
{
    for (int i = 0; i < 100000; i++)
    {
        switch (fork())
        {
        case -1:
            errExit("fork");
        case 0:
            wait(NULL);
            break;        
        default:
            __exit(EXIT_SUCCESS);
            break;
        }
    }

    return 0;
}