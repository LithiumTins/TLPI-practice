#define _POSIX_C_SOURCE 200809L

#include <sys/wait.h>
#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{
    int status;
    pid_t childPid;

    if (argc > 1 && strcmp(argv[1], "--help") == 0)
        usageErr("%s [exit-status]\n", argv[0]);

    switch (fork()) {
    case -1: errExit("fork");

    case 0:             /* Child: either exits immediately with given
                           status or loops waiting for signals */
        printf("Child started with PID = %ld\n", (long) getpid());
        if (argc > 1)                   /* Status supplied on command line? */
            exit(getInt(argv[1], 0, "exit-status"));
        else                            /* Otherwise, wait for signals */
            for (;;)
                pause();
        exit(EXIT_FAILURE);             /* Not reached, but good practice */

    default:            /* Parent: repeatedly wait on child until it
                           either exits or is terminated by a signal */
        for (;;) {
            siginfo_t info;
            childPid = waitid(P_ALL, 0, &info, WEXITED | WSTOPPED | WCONTINUED);
            if (childPid == -1)
                errExit("waitid");

            /* Print status in hex, and as separate decimal bytes */
            status = info.si_status;
            printf("waitid() returned: PID=%ld; status=0x%04x (%d,%d)\n",
                    (long) childPid,
                    (unsigned int) status, status >> 8, status & 0xff);

            switch (info.si_code)
            {
            case CLD_EXITED:
                printf("child exited, status=%d\n", info.si_status);
                break;
            case CLD_KILLED:
                printf("child killed by signal %d (%s)",
                        WTERMSIG(status), strsignal(status));
                printf("\n");
                break;
            case CLD_STOPPED:
                printf("child stopped by signal %d (%s)\n",
                        WSTOPSIG(status), strsignal(status));
                break;
            case CLD_CONTINUED:
                printf("child continued\n");
                break;
            default:
                printf("what happened to this child? (status=%x)\n",
                            (unsigned int) status);
                break;
            }

            if (WIFEXITED(status) || WIFSIGNALED(status))
                exit(EXIT_SUCCESS);
        }
    }
}