#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "tlpi_hdr.h"

char *realpath(const char *path, char *resolve_path)
{
    if (resolve_path == NULL)
        resolve_path = calloc(PATH_MAX, sizeof(char));

    char _path[PATH_MAX + 1];
    if (strncpy(_path, path, PATH_MAX) == NULL)
        errExit("strncpy");
    _path[PATH_MAX] = '\0';

    int offset = 0;
    if (_path[0] != '/')
    {
        if (getcwd(resolve_path, PATH_MAX) == NULL)
            errExit("getcwd");
        offset += strlen(resolve_path);
    }

    for (char *p = _path; *p != '\0'; p++)
        if (*p == '/')
            *p = '\0';
    
    for (char *p = _path; *p != '\0'; p += strlen(p) + 1)
    {
        resolve_path[offset++] = '/';
        strcpy(resolve_path + offset, p);

        struct stat st;
        if (lstat(resolve_path, &st) == -1)
            errExit("lstat");
        if (S_ISLNK(st.st_mode))
        {
            char buffer[PATH_MAX];
            ssize_t link_len;
            if ((link_len = readlink(resolve_path, buffer, PATH_MAX)) == -1)
                errExit("readlink");
            buffer[link_len] = '\0';
            if (buffer[0] == '/')
                strcpy(resolve_path, buffer);
            else
                strcpy(resolve_path + offset, buffer);
        }

        offset = strlen(resolve_path);
    }

    return resolve_path;
}

int main(int argc, char *argv[])
{
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file\n", argv[0]);

    printf("%s\n", realpath(argv[1], NULL));

    return 0;
}