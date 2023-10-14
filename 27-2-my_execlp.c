#define _XOPEN_SOURCE 500

#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

extern char ** environ;

int my_execlp(const char *file, const char *arg, ...)
{
    va_list ap;
    char **argv;
    char *str;
    int argc;

    va_start(ap, arg);
    argc = 0;
    while ((str = va_arg(ap, char *)) != NULL) argc++;
    va_end(ap);
    
    va_start(ap, arg);
    argv = (char **)malloc(sizeof(char *) * (argc + 1));
    for (int i = 0; i < argc; i++) argv[i] = strdup(va_arg(ap, char *));
    argv[argc] = NULL;
    
    execve(file, argv, environ);
    return -1;
}