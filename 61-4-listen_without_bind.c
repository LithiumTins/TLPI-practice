#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "tlpi_hdr.h"

int main(int argc, char argv[])
{
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
        errExit("socket");

    if (listen(sfd, 1) == -1)
        errExit("listen");

    struct sockaddr_in *addr;
    socklen_t len = sizeof(struct sockaddr_in);
    if (getsockname(sfd, addr, &len) == -1)
        errExit("getsockname");
    
    char buf[16] = { 0 };
    if (inet_ntop(AF_INET, addr, buf, 16) == -1)
        errExit("inet_ntop");

    printf("address is %s\nport is %d\n", buf, addr->sin_port);

    return 0;
}