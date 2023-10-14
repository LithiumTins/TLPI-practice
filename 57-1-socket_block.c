#include <sys/types.h>
#include <sys/socket.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    int sv[2];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1)
        errExit("socketpair");

    switch (fork())
    {
    case -1:
        errExit("fork");
        break;
    case 0:
        int cnt = 0;
        while (1)
        {
            char buf[4096];
            printf("send ");
            fflush(NULL);
            if (write(sv[0], buf, 4096) == -1)
                errExit("write");
            printf(" over %d\n", cnt++);
        }
    default:
        while (1)
        {
            char buf[4096];
            sleep(1);
            printf("read ");
            fflush(NULL);
            int read_num;
            if ((read_num = read(sv[1], buf, 4096)) < 0)
                exit(EXIT_FAILURE);
            printf("%d ", read_num);
        }
        break;
    }

    return 0;
}