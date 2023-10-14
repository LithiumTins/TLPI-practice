#define _POSIX_C_SOURCE 200112L

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

int inetConnect(const char *host, const char *service, int type)
{
    struct addrinfo hint, *result;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNIX;
    hint.ai_socktype = type;

    if (getaddrinfo(host, service, &hint, &result) != 0)
    {
        errno = ENOSYS;
        return -1;
    }
    
    int sfd;
    for (struct addrinfo *p = result; p != NULL; p = p->ai_next)
    {
        sfd = socket(AF_UNSPEC)
    }
}

int inetListen(const char *service, int backlog, socklen_t *addrlen);

int inetBind(const char *service, int type, socklen_t *addrlen);

char *inetAddressStr(const struct sockaddr *addr, socklen_t addrlen, char *addrStr, int addrStrLen);