#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include "tlpi_hdr.h"

int Socket(void)
{
    int fd;
    if ((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
        errExit("socket");
    return fd;
}

void Bind(int fd, __CONST_SOCKADDR_ARG addr, socklen_t len)
{
    if (bind(fd, addr, len) == -1)
        errExit("bind");
}

void Remove(const char *filename)
{
    if (remove(filename) == -1 && errno != ENOENT)
        errExit("remove");
}

void Connect(int fd, __CONST_SOCKADDR_ARG addr, socklen_t len)
{
    if (connect(fd, addr, len) == -1)
        errExit("connect");
}

int main(int argc, char *argv[])
{
    int a, b, c;

    a = Socket();
    b = Socket();
    c = Socket();

    struct sockaddr_un sa;
    memset(&sa, 0, sizeof(sa));
    sa.sun_family = AF_UNIX;

    strncpy(sa.sun_path, "/tmp/socket_a", sizeof(sa.sun_path));
    Remove("/tmp/socket_a");
    Bind(a, (struct sockaddr *)&sa, sizeof(sa));

    strncpy(sa.sun_path, "/tmp/socket_b", sizeof(sa.sun_path));
    Remove("/tmp/socket_b");
    Bind(b, (struct sockaddr *)&sa, sizeof(sa));

    strncpy(sa.sun_path, "/tmp/socket_c", sizeof(sa.sun_path));
    Remove("/tmp/socket_c");
    Bind(c, (struct sockaddr *)&sa, sizeof(sa));

    printf("Connect a to b\n");
    Connect(a, (struct sockaddr *)&sa, sizeof(sa));
    printf("Succeeded\n");

    strncpy(sa.sun_path, "/tmp/socket_a", sizeof(sa.sun_path));
    printf("Connect c to a\n");
    Connect(c, (struct sockaddr *)&sa, sizeof(sa));
    printf("Succeeded\n");

    return 0;
}

#include <netinet/in.h>