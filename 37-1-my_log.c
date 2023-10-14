#include <syslog.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int level = 0;
    switch (argv[1][0])
    {
    case 'E':
        level = (argv[1][1] == 'M') ? LOG_EMERG : LOG_ERR;
        break;
    case 'A':
        level = LOG_ALERT;
        break;
    case 'C':
        level = LOG_CRIT;
        break;
    case 'W':
        level = LOG_WARNING;
        break;
    case 'N':
        level = LOG_NOTICE;
        break;
    case 'I':
        level = LOG_INFO;
        break;
    case 'D':
        level = LOG_DEBUG;
        break;
    default:
        printf("Invalid level!\n");
        exit(EXIT_FAILURE);
    }

    syslog(level, "%s", argv[2]);

    return 0;
}