#include <termios.h>
#include <unistd.h>

int my_isatty(int fd)
{
    struct termios tm;
    if (tcgetattr(fd, &tm) != -1)
        return 1;
    return 0;
}