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
void clear_file(void);
void handler(int sig);

static int serverId;
struct client *header;

int main(int argc, char *argv[])
{
    int fd;
    fd = open(KNOWN_FILE, O_CREAT | O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR);
    if (fd == -1)
        errExit("open");
    
    if (atexit(clear_file) == -1)
        errExit("atexit - file");
    
    serverId = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR);
    if (serverId == -1)
        errExit("msgget");

    if (atexit(clear_msq) == -1)
        errExit("atexit - msq");

    if (write(fd, &serverId, sizeof(serverId)) == -1)
        errExit("write");
    
    if (close(fd) == -1)
        errExit("close");

    struct sigaction sa;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    if (sigaction(SIGTERM, &sa, NULL) == -1)
        errExit("sigaction - SIGTERM");
    if (sigaction(SIGINT, &sa, NULL) == -1)
        errExit("sigaction - SIGINT");
    if (sigaction(SIGALRM, &sa, NULL) == -1)
        errExit("sigaction - SIGALRM");

    if ((header = malloc(sizeof(struct client))) == NULL)
        errExit("malloc");
    header->next = NULL;

    while (1)
    {
        struct clientMsg msg;
        ssize_t msg_len;
        msg_len = msgrcv(serverId, &msg, CLI_LEN, -3, 0);
        if (msg_len == -1)
        {
            if (errno == EINTR)
                continue;
            else
                errExit("msgrcv");
        }
        if (msg.mtype == LOGIN_MSG)
        {
            struct client *tmp = malloc(sizeof(struct client));
            if (tmp == NULL)
                errExit("malloc");
            tmp->pid = msg.pid;
            tmp->msgid = *(int *)msg.mtext;
            tmp->next = header->next;
            header->next = tmp;
        }
        else if (msg.mtype == LOGOUT_MSG)
        {
            struct client *p = header;
            while (p->next)
            {
                if (p->next->pid == msg.pid)
                {
                    struct client *tmp = p->next;
                    p->next = tmp->next;
                    free(tmp);
                    break;
                }
                p = p->next;
            }
        }
        else if (msg.mtype == TEXT_MSG)
        {
            struct client *p = header;
            while (p->next)
            {
                alarm(2);
                if (msgsnd(p->next->msgid, &msg, msg_len, 0) == -1)
                {
                    if (errno == EINTR)
                    {
                        struct client *tmp = p->next;
                        p->next = tmp->next;
                        if (msgctl(tmp->msgid, IPC_RMID, NULL) == -1)
                            fprintf(stderr, "fail to close message queue of process %d\n", tmp->pid);
                        free(tmp);
                        continue;
                    }
                    else
                        errExit("msgsnd");
                }
                alarm(0);
                p = p->next;
            }
        }
    }

    return 0;
}

void clear_msq(void)
{
    msgctl(serverId, IPC_RMID, NULL);
}

void clear_file(void)
{
    unlink(KNOWN_FILE);
}

void handler(int sig)
{
    if (sig == SIGALRM)
        return;
    
    msgctl(serverId, IPC_RMID, NULL);
    unlink(KNOWN_FILE);

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