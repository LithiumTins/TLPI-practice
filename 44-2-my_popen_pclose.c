#define _POSIX_C_SOURCE 1
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

struct p_node
{
    FILE *file;
    pid_t pid;
    struct p_node *next; 
};

struct p_node *p_header = NULL;

FILE *popen(const char *commmand, const char *type)
{
    if (p_header == NULL)
        p_header = calloc(1, sizeof(struct p_node));

    int isRead;
    if (strcmp(type, "r") == 0) isRead = 1;
    else if (strcmp(type, "w") == 0) isRead = 0;
    else return NULL;

    int pfd[2], child;
    if (pipe(pfd) == -1)
        return NULL;
    switch (child = fork())
    {
    case -1:
        return NULL;
    case 0: // Child
        struct p_node *p = p_header->next;
        while (p)
        {
            fclose(p->file);
            struct p_node *tmp = p;
            p = p->next;
            free(tmp);
        }
        if (isRead)
        {
            close(pfd[0]);
            dup2(pfd[1], STDOUT_FILENO);
            close(pfd[1]);
        }
        else
        {
            close(pfd[1]);
            dup2(pfd[0], STDIN_FILENO);
            close(pfd[0]);
        }
        system(commmand);
    default:
        struct p_node *tmp = calloc(1, sizeof(struct p_node));
        if (isRead)
        {
            close(pfd[1]);
            FILE *fp = fdopen(pfd[0], "r");
            tmp->file = fp;
        }
        else 
        {
            close(pfd[0]);
            FILE *fp = fdopen(pfd[1], "w");
            tmp->file = fp;
        }
        tmp->pid = child;
        tmp->next = p_header->next;
        p_header->next = tmp;
        return tmp->file;
    }
}

int pclose(FILE *stream)
{
    struct p_node *p = p_header;
    while (p->next && p->next->file != stream)
        p = p->next;
    if (!p->next)
        return -1;
    else
    {
        fclose(stream);
        int status;
        if (waitpid(p->pid, &status, 0) == -1)
            status = -1;
        struct p_node *tmp = p->next;
        p->next = tmp->next;
        free(tmp);
        return status;
    }
}