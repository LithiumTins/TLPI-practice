#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

void abort(void)
{
    while (1)
    {
        kill(getpid(), SIGABRT);
        signal(SIGABRT, SIG_DFL);
    }
}