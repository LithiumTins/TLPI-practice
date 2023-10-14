#include <unistd.h>
#include <ctype.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    int pfd1[2], pfd2[2];

    if (pipe(pfd1) == -1)
        errExit("pipe");
    if (pipe(pfd2) == -1)
        errExit("pipe");

    printf("opne pipe on fd (%d, %d) and (%d, %d)\n", pfd1[0], pfd1[1], pfd2[0], pfd2[1]);

    char buf[4096];
    switch (fork())
    {
    case -1:
        errExit("fork");
        break;
    case 0: // Child
        if (close(pfd1[1]) == -1)
            errExit("close");
        if (close(pfd2[0]) == -1)
            errExit("close");
        while (1)
        {
            int read_num = read(pfd1[0], buf, 4096);
            if (read_num == -1)
                errExit("child read");
            else if (read_num == 0)
                break;
            else 
            {
                for (int i = 0; buf[i] != '\0' && i < read_num; i++)
                    buf[i] = toupper(buf[i]);
                if (write(pfd2[1], buf, read_num) == -1)
                    errExit("write");
            }
        }
    default:    // parent
        if (close(pfd1[0]) == -1)
            errExit("close");
        if (close(pfd2[1]) == -1)
            errExit("close");
        while (1) 
        {
            fgets(buf, 4096, stdin);
            if (write(pfd1[1], buf, strlen(buf) + 1) == -1)
                errExit("write");
            int read_num = read(pfd2[0], buf, 4096);
            if (read_num == -1)
                errExit("read");
            else if (read_num == 0)
                break;
            else
            {
                printf("%s", buf);
            }
        }
        break;
    }

    return 0;
}