#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

int my_pipe(int pipefd[2])
{
    int fd[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd) == -1)
        return -1;
    
    if (shutdown(fd[0], SHUT_WR) == -1)
    {
        close(fd[0]);
        close(fd[1]);
        return -1;
    }
        
    pipefd[0] = fd[0];
    pipefd[1] = fd[1];

    return 0;
}