#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "tlpi_hdr.h"
//#include <pwd.h>

// forward declaration (to be deleted)
typedef int uid_t;
typedef int gid_t;
struct passwd
{
    char *pw_name;
    char *pw_passwd;
    uid_t pw_uid;
    gid_t pw_gid;
    char *pw_gecos;
    char *pw_dir;
    char *pw_shell;
};
struct passwd *getpwnam(const char *name);
// over

int main(int argc, char *argv[])
{
    char *username;
    struct passwd *pw;
    uid_t uid;
    DIR *dir;
    struct dirent *ent;
    char filename[1024] = {};

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s username\n", argv[0]);
    
    username = argv[1];

    pw = getpwnam(username);
    if (pw == NULL)
        fatal("couldn't get password record");
    uid = pw->pw_uid;

    dir = opendir("/proc");
    if (dir == NULL)
        errExit("opendir");

    for (;;)
    {
        int fd;
        int read_num;
        char buffer[1024];
        char line[1024];

        errno = 0;
        ent = readdir(dir);
        if (ent == NULL)
            break;

        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;
        
        if (snprintf(filename, sizeof(filename), "/proc/%s/status", ent->d_name) < 0)
            fatal("snprintf");
        
        fd = open(filename, O_RDONLY);
        if (fd == -1)
            continue;
        
        char name[100];
        int i = 0;
        int j = 0;
        int over = 0;
        while ((read_num = read(fd, buffer, sizeof(buffer))) != -1)
        {
            if (read_num == 0)
                break;
            
            for (i = 0; i < sizeof(buffer); i++)
            {
                if (buffer[i] == '\n')
                {
                    char *s;
                    int this_uid;
                    line[j] = '\0';
                    s = strstr(line, "Name:");
                    if (s)
                        strcpy(name, s + 6);
                    s = strstr(line, "Uid:");
                    if (s)
                    {
                        over = 1;
                        this_uid = atoi(s + 5);
                        if (this_uid == uid)
                            printf("%s\n", name);
                        else
                            break;
                    }
                    j = 0;
                }
                line[j++] = buffer[i];
            }
            if (over) break;
            line[j] = '\0';
        }

        if (read_num == -1)
            errExit("read");

        if (close(fd) == -1)
            errExit("close");
    }

    if (errno != 0)
        errExit("readdir");

    return 0;
}