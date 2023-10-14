#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "tlpi_hdr.h"

#define MAX_PID 32768

struct node
{
    int pid;
    int ppid;
    char *ins;
    struct node *child;
    struct node *next;
};
struct node *process[MAX_PID];

char mark[1024];
void create_node(int pid, int ppid, char *ins)
{
    if (process[pid] && process[pid]->pid != -1) return;
    if (process[pid] == NULL)
    {
        process[pid] = (struct node *)malloc(sizeof(struct node));
        process[pid]->pid = pid;
        process[pid]->child = (struct node *)malloc(sizeof(struct node));
        process[pid]->child->next = NULL;
    }
    process[pid]->ppid = ppid;
    process[pid]->ins = (char *)malloc(strlen(ins) + 1);
    strcpy(process[pid]->ins, ins);
    if (process[ppid] == NULL)
    {
        process[ppid] = (struct node *)malloc(sizeof(struct node));
        process[ppid]->pid = ppid;
        process[ppid]->ppid = -1;
        process[ppid]->child = (struct node *)malloc(sizeof(struct node));
        process[ppid]->child->next = NULL;
        process[ppid]->ins = NULL;
    }
    process[pid]->next = process[ppid]->child->next;
    process[ppid]->child->next = process[pid];
}

void print_tree(struct node *root, int blank_num, char *pre, int print)
{
    for (int i = 0; i < print * blank_num; i++)
        printf("%c", mark[i]);
    char buffer[1024];
    sprintf(buffer, "%s(%d)", root->ins, root->pid);
    printf("%s%s", pre, buffer);
    blank_num += strlen(buffer) + (root->pid == 1 ? 1 : 3);
    struct node *p = root->child->next;
    if (!p)
    {
        printf("\n");
        return;
    }
    else if (p->next)
    {
        mark[blank_num] = '|';
        print_tree(p, blank_num, "-+-", 0);
    }
    else
        print_tree(p, blank_num, "---", 0);
    p = p->next;
    while (p)
    {
        if (p->next)
            print_tree(p, blank_num, "|-", 1);
        else
            print_tree(p, blank_num, "`-", 1);
        p = p->next;
    }
    mark[blank_num] = ' ';
}

int main(int argc, char *argv[])
{
    for (int i = 0; i < 1024; i++) mark[i] = ' ';

    DIR *dir = opendir("/proc");
    if (dir == NULL)
        errExit("opendir");
    while (true)
    {
        errno = 0;
        struct dirent *ent = readdir(dir);
        if (ent == NULL)
        {
            if (errno == 0)
                break;
            else
                errExit("readdir");
        }
        if (strcmp(ent->d_name, "self") == 0 || strcmp(ent->d_name, "thread-self") == 0)
            continue;
        char buffer[1024] = {};
        if (snprintf(buffer, sizeof(buffer), "/proc/%s/status", ent->d_name) == -1)
            fatal("snprintf failed");
        int fd = open(buffer, O_RDONLY);
        if (fd == -1)
            continue;
        if (read(fd, buffer, sizeof(buffer)) == -1)
            errExit("read");
        close(fd);
        int pid = atoi(strstr(buffer, "Pid:") + 5);
        int ppid = atoi(strstr(buffer, "PPid:") + 6);
        char *enter = strchr(strstr(buffer, "Name:") + 6, '\n');
        *enter = '\0';
        create_node(pid, ppid, strstr(buffer, "Name:") + 6);
    }
    
    for (int i = 1; i < MAX_PID; i++)
    {
        if (process[i] && process[i]->ppid == -1)
        {
            struct node *tmp;
            while (tmp = process[i]->child->next)
            {
                process[i]->child->next = tmp->next;
                tmp->next = process[1]->child->next;
                process[1]->child->next = tmp;
            }
        }
    }

    print_tree(process[1], 0, "", 0);

    for (int i = 0; i < MAX_PID; i++)
    {
        if (process[i])
        {
            free(process[i]->ins);
            free(process[i]->child);
            free(process[i]);
        }
    }

    return 0;
}