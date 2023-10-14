#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int setenv(const char *name, const char *value, int overwrite)
{
    if (!getenv(name) && !overwrite) return 0;

    int len_name = strlen(name), len_value = strlen(value);
    char * buffer = (char *)malloc(len_name + len_value + 2);

    memcpy(buffer, name, len_name);
    buffer[len_name] = '=';
    memcpy(buffer + len_name + 1, value, len_value + 1);

    return putenv(buffer);
}

int unsetenv(const char *name)
{
    int i = 0, j = 0;
    while (environ[j])
    {
        if (!strstr(environ[j], name)) environ[i++] = environ[j];
        j++;
    }
    while (i < j) environ[i] = NULL;
    return 0;
}

int main(int argc, char *argv[])
{
    return 0;
}