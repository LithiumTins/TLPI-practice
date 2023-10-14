#define _BSD_SOURCE
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    char *username = "root", *password, *encrypted;
    struct passwd *pwd;
    int path = 1;

    if (strcmp(argv[1], "-u") == 0)
    {
        username = argv[2];
        path = 3;
    }

    password = getpass("Password: ");

    pwd = getpwnam("username");
    if (pwd == NULL)
        errExit("getpwnam");

    encrypted = crypt(password, pwd->pw_passwd);
    for (char *p = password; *p != '\0'; )
        *p++ = '\0';
    if (encrypted == NULL)
        errExit("crypt");
    
    if (strcmp(encrypted, pwd->pw_passwd) != 0)
        fatal("wrong password\n");
    
    setuid(pwd->pw_uid);
    execv(argv[path], argv + path);
    errExit("execv");

    return 0;
}