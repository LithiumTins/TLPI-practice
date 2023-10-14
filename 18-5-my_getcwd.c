#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include "tlpi_hdr.h"

struct dir_node
{
    char *dir;
    struct dir_node *next;
};

char *m_getcwd(char *buf, size_t size)
{
    if (buf == NULL)
        buf = malloc(size);

    struct dir_node *header = malloc(sizeof(struct dir_node));
    header->dir = NULL;
    header->next = NULL;

    while (1)
    {
        struct stat st_local;
        struct stat st_upper;
        if (stat(".", &st_local) == -1 || stat("..", &st_upper) == -1)
            errExit("stat");
        if (st_local.st_dev == st_upper.st_dev && st_local.st_ino == st_upper.st_ino)
            break;

        if (chdir("..") == -1)
            errExit("chdir");
        DIR *upper = opendir(".");
        struct dirent *ent;
        while (1)
        {
            errno = 0;
            if ((ent = readdir(upper)) == NULL)
            {
                if (errno == 0)
                    fatal("unable to find the ent before looking all through the upper directory\n");
                else 
                    errExit("readdir");
            }

            struct stat st_ent;
            if (lstat(ent->d_name, &st_ent) == -1)
                errExit("lstat");
            if (st_ent.st_dev == st_local.st_dev && st_ent.st_ino == st_local.st_ino)
            {
                struct dir_node *local = malloc(sizeof(struct dir_node));
                local->dir = strdup(ent->d_name);
                local->next = header->next;
                header->next = local;
                break;
            }
        }
    }

    int offset = 0;
    for (struct dir_node *p = header->next; p != NULL; p = p = header->next)
    {
        if (offset + strlen(p->dir) + 1 >= size)
        {
            errno = ERANGE;
            return NULL;
        }
        buf[offset++] = '/';
        strcpy(buf + offset, p->dir);
        offset = strlen(buf);

        header->next = p->next;
        free(p->dir);
        free(p);
    }

    free(header);
    return buf;
}

int main(int argc, char *argv[])
{
    printf("%s\n", m_getcwd(NULL, PATH_MAX));

    return 0;
}