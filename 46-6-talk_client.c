#define _GNU_SOURCE
#define KNOWN_FILE "/tmp/ser_msq"
#define MAX_TEXT_LEN 512
#define LOGIN_MSG 1
#define LOGOUT_MSG 2
#define TEXT_MSG 3

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stddef.h>
#include "tlpi_hdr.h"

struct clientMsg
{
    long mtype;
    pid_t pid;
    char mtext[MAX_TEXT_LEN];
};

struct client
{
    pid_t pid;
    int msgid;
    struct client *next;
};

#define CLI_LEN (offsetof(struct clientMsg, mtext) - offsetof(struct clientMsg, pid) + MAX_TEXT_LEN)

void clear_msq(void);
void log_out(void);
void handler(int sig);

static int serverId;
static int clientId;
static int isChild = 0;

int main(int argc, char *argv[])
{
    int fd;
    fd = open(KNOWN_FILE, O_RDONLY);
    if (fd == -1)
        errExit("open");
    
    if (read(fd, &serverId, sizeof(serverId)) == -1)
        errExit("read");

    clientId = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR);
    if (clientId == -1)
        errExit("msgget");

    if (atexit(clear_msq) == -1)
        errExit("atexit - msq");
    
    if (close(fd) == -1)
        errExit("close");

    struct sigaction sa;
    sa.sa_flags = SA_RESTART;
    sigfullset(&sa.sa_mask);
    sigdelset(&sa.sa_mask, SIGALRM);
    sa.sa_handler = handler;
    if (sigaction(SIGTERM, &sa, NULL) == -1)
        errExit("sigaction - SIGTERM");
    if (sigaction(SIGINT, &sa, NULL) == -1)
        errExit("sigaction - SIGINT");
    if (sigaction(SIGALRM, &sa, NULL) == -1)
        errExit("sigaction - SIGALRM");

    struct clientMsg msg;

    msg.mtype = LOGIN_MSG;
    msg.pid = getpid();
    *(int *)msg.mtext = clientId;
    alarm(2);
    if (msgsnd(serverId, &msg, offsetof(struct clientMsg, mtext) - offsetof(struct clientMsg, pid) + sizeof(int), 0) == -1)
    {
        if (errno == EINTR)
            fatal("unable to log in to server");
        else
            errExit("msgsnd");
    }
    alarm(0);

    switch (fork())
    {
    case -1:
        errExit("fork");
        break;
    case 0:
        isChild = 1;
        while (1)
        {
            ssize_t msg_len;
            alarm(300);
            msg_len = msgrcv(clientId, &msg, CLI_LEN, 0, 0);
            alarm(0);
            if (msg_len == -1)
            {
                if (errno == EINTR)
                    fprintf(stderr, "No message from server!\n");
                else
                    errExit("msgrcv");
            }
            printf("%ld: %s\n\n", msg.pid, msg.mtext);
        }
    default:
        msg.mtype = TEXT_MSG;
        while (1)
        {
            fgets(msg.mtext, 512, stdin);
            alarm(2);
            if (msgsnd(serverId, &msg, CLI_LEN, 0) == -1)
            {
                if (errno == EINTR)
                    fatal("Unable to sent text to the server");
                else 
                    errExit("msgsnd");
            }
            alarm(0);
        }
        break;
    }

    return 0;
}

void clear_msq(void)
{
    if (!isChild)
        msgctl(clientId, IPC_RMID, NULL);
}

void log_out(void)
{
    if (isChild)
        return;
    struct clientMsg msg;
    msg.mtype = LOGOUT_MSG;
    msg.pid = getpid();
    alarm(2);
    msgsnd(serverId, &msg, sizeof(msg.pid), 0);
    alarm(0);
}

void handler(int sig)
{
    if (sig == SIGALRM)
        return;
    
    if (!isChild)
    {
        msgctl(clientId, IPC_RMID, NULL);
    
        struct clientMsg msg;
        msg.mtype = LOGOUT_MSG;
        msg.pid = getpid();
        alarm(2);
        msgsnd(serverId, &msg, sizeof(msg.pid), 0);
        alarm(0);
    }

    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sigaction(sig, &sa, NULL);

    raise(sig);

    sigset_t mask;
    sigprocmask(0, NULL, &mask);
    sigdelset(&mask, sig);
    sigprocmask(SIG_SETMASK, &mask, NULL);
}