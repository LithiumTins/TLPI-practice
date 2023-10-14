#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include "tlpi_hdr.h"

struct file_node
{
    char *filename;
    struct file_node *next;
};

struct file_node *arr;

void init()
{
    arr = malloc(sizeof(struct file_node));
    arr->filename = NULL;
    arr->next = NULL;
}

void insert(char *filename)
{
    struct file_node *p = arr->next;
    struct file_node *pre = arr;
    while (1)
    {
        if (!p || strcmp(filename, p->filename) < 0)
        {
            struct file_node *tmp = malloc(sizeof(struct file_node));
            tmp->filename = malloc(8 * sizeof(char));
            strcpy(tmp->filename, filename);
            tmp->next = p;
            pre->next = tmp;
            break;
        }
        p = p->next;
        pre = pre->next;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s NF (1000 <= NF <= 20000)\n", argv[0]);

    int nf = atoi(argv[1]);
    if (nf < 1000 || nf > 20000)
        usageErr("%s NF (1000 <= NF <= 20000)\n", argv[0]);

    srand(nf);
    init();
    struct timeval start_time;
    if (gettimeofday(&start_time, NULL) == -1)
        errExit("gettimeofday");

    for (int i = 0; i < nf; )
    {
        char filename[8];
        int code = rand() % 1000000;
        snprintf(filename, 8, "x%06d", code);
        errno = 0;
        int fd = open(filename, O_WRONLY | O_CREAT | O_EXCL, 
            S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd == -1)
        {
            if (errno == EEXIST)
                continue;
            else
                errExit("open");
        }
        if (write(fd, "1", 1) == -1)
            errExit("write");
        close(fd);
        insert(filename);
        i++;
    }

    struct file_node *p = arr->next;
    while (p)
    {
        if (unlink(p->filename) == -1)
            errExit("unlink");
        p = p->next;
    }

    struct timeval end_time;
    if (gettimeofday(&end_time, NULL) == -1)
        errExit("gettimeofday");
    printf("time: %lld", ((long long)end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_usec - start_time.tv_usec));

    p = arr;
    while (p->next)
    {
        struct file_node *tmp = p->next->next;
        free(p->next->filename);
        free(p->next);
        p->next = tmp;
    }
    free(arr);

    return 0;
}
