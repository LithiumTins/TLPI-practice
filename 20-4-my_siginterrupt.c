#include <signal.h>

int m_siginterrupt(int sig, int flag)
{
    struct sigaction act;
    if (sigaction(sig, NULL, &act) == -1)
        return -1;
    if (flag)
        act.sa_flags |= SA_RESTART;
    else
        act.sa_flags &= ~SA_RESTART;
    if (sigaction(sig, &act, NULL) == -1)
        return -1;
    return 0;
}

int main(int argc, char *argv[])
{
    return 0;
}