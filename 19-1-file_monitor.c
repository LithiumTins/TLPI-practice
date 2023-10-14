#define _XOPEN_SOURCE 500
#include "useful.h"
#include <ftw.h>
#include <sys/inotify.h>
#include <limits.h>

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

struct node
{
    int wd;
    char *fpath;
    struct node *next;
};

int inotify_fd;
struct node *header;
char buffer[BUF_LEN];

int inotify_adder(const char *fpath, const struct stat *sb, 
                    int typeflag, struct FTW *ftwbuf)
{
    int wd = inotify_add_watch(inotify_fd, fpath, IN_ALL_EVENTS);
    if (wd == -1)
        errExit("inotify_add_watch");
    struct node *tmp = malloc(sizeof(struct node));
    if (tmp == NULL)
        errExit("malloc");
    tmp->wd = wd;
    tmp->fpath = strdup(fpath);
    if (tmp->fpath == NULL)
        errExit("strdup");
    tmp->next = header->next;
    header->next = tmp;
    return 0;
}

static void             /* Display information from inotify_event structure */
displayInotifyEvent(struct inotify_event *i)
{
    printf("    wd =%2d; ", i->wd);
    if (i->cookie > 0)
        printf("cookie =%4d; ", i->cookie);

    printf("mask = ");
    if (i->mask & IN_ACCESS)        printf("IN_ACCESS ");
    if (i->mask & IN_ATTRIB)        printf("IN_ATTRIB ");
    if (i->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (i->mask & IN_CLOSE_WRITE)   printf("IN_CLOSE_WRITE ");
    if (i->mask & IN_CREATE)        printf("IN_CREATE ");
    if (i->mask & IN_DELETE)        printf("IN_DELETE ");
    if (i->mask & IN_DELETE_SELF)   printf("IN_DELETE_SELF ");
    if (i->mask & IN_IGNORED)       printf("IN_IGNORED ");
    if (i->mask & IN_ISDIR)         printf("IN_ISDIR ");
    if (i->mask & IN_MODIFY)        printf("IN_MODIFY ");
    if (i->mask & IN_MOVE_SELF)     printf("IN_MOVE_SELF ");
    if (i->mask & IN_MOVED_FROM)    printf("IN_MOVED_FROM ");
    if (i->mask & IN_MOVED_TO)      printf("IN_MOVED_TO ");
    if (i->mask & IN_OPEN)          printf("IN_OPEN ");
    if (i->mask & IN_Q_OVERFLOW)    printf("IN_Q_OVERFLOW ");
    if (i->mask & IN_UNMOUNT)       printf("IN_UNMOUNT ");
    printf("\n");

    if (i->len > 0)
        printf("        name = %s\n", i->name);
}

int main(int argc, char *argv[])
{
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s dir\n", argv[0]);

    struct stat dir;
    if (stat(argv[1], &dir) == -1)
        errExit("stat");
    if (!S_ISDIR(dir.st_mode))
        usageErr("%s dir", argv[0]);

    inotify_fd = inotify_init();
    if (inotify_fd == -1)
        errExit("inofity_init");

    header = malloc(sizeof(struct node));
    header->next = NULL;

    if (nftw(argv[1], inotify_adder, 20, 0) == -1)
        errExit("nftw");

    while (1)
    {
        int read_num = read(inotify_fd, buffer, BUF_LEN);
        if (read_num == -1)
            errExit("read");
        for (char *p = buffer; p < buffer + read_num;)
        {
            struct inotify_event *event = (struct inotify_event *)p;
            displayInotifyEvent(event);
            p += sizeof(struct inotify_event) + event->len;
        }
    }

    return 0;
}