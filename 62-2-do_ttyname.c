#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <linux/limits.h>
#include <string.h>

char *my_ttyname(int fd)
{
    static char res[PATH_MAX];

    memset(res, 0, PATH_MAX);
    strcpy(res, "/dev/");

    if (!isatty(fd))
        return NULL;

    struct stat fd_stat;
    if (fstat(fd, &fd_stat) == -1)
        return NULL;

    DIR *dev_dir = opendir("/dev");
    if (dev_dir == NULL)
        return NULL;
    
    while (1)
    {
        struct dirent *ent;
        errno = 0;
        ent = readdir(dev_dir);
        if (ent == NULL)
        {
            if (errno == 0)
                break;
            else
                return NULL;
        }

        char buf[PATH_MAX] = "/dev/";
        strcpy(buf + 5, ent->d_name);

        struct stat st;
        if (stat(buf, &st) == -1)
            return NULL;

        if (st.st_rdev == fd_stat.st_rdev)
        {
            strcpy(res + 5, ent->d_name);
            return res;
        }
    }

    closedir(dev_dir);
    dev_dir = opendir("/dev/pts");
    strcpy(res + 4, "/pts");

    while (1)
    {
        struct dirent *ent;
        errno = 0;
        ent = readdir(dev_dir);
        if (ent == NULL)
        {
            if (errno == 0)
                break;
            else
                return NULL;
        }

        char buf[PATH_MAX] = "/dev/pts/";
        strcpy(buf + 9, ent->d_name);

        struct stat st;
        if (stat(buf, &st) == -1)
            return NULL;

        if (st.st_rdev == fd_stat.st_rdev)
        {
            strcpy(res + 9, ent->d_name);
            return res;
        }
    }

    return NULL;
}