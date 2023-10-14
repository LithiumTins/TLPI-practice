#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s filepath\n", argv[0]);

    DIR *dir_proc = opendir("/proc");
    if (dir_proc == NULL)
        errExit("opendir on /proc");
    
    struct dirent *proc;
    while (true)
    {
        errno = 0;
        if ((proc = readdir(dir_proc)) == NULL)
        {
            if (errno)
                errExit("readdir on /proc");
            else
                break;
        }

        int len = strlen(proc->d_name);
        for (int i = 0; i < len; i++)
            if (!isdigit(proc->d_name[i]))
                continue;

        char buffer[PATH_MAX];
        sprintf(buffer, "/proc/%s", proc->d_name);
        DIR *dir_fd = opendir(buffer);
        if (dir_fd == NULL)
        {
            char tmp[1024];
            sprintf(tmp, "opendir on %s", buffer);
            errExit(tmp);
        }

        struct dirent *fd;
        while (true)
        {
            errno = 0;
            if ((fd = readdir(dir_fd)) == NULL)
            {
                if (errno)
                    errExit("readdir on /fd");
                else
                    break;
            }

            char ano_buffer[PATH_MAX];
            char link_address[PATH_MAX];
            sprintf(ano_buffer, "%s/fd/%s", buffer, fd->d_name);
            ssize_t link_len;
            if ((link_len = readlink(ano_buffer, link_address, PATH_MAX)) == -1)
                errExit("readlink");
            link_address[link_len] = '\0';
            
            if (strcmp(argv[1], link_address) == 0)
            {
                printf("%s\n", proc->d_name);
                break;
            }
        }
    }

    return 0;
}