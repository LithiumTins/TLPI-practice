#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>

key_t my_ftok(const char *pathname, int proj_id)
{
    struct stat buf;
    if (stat(pathname, &buf) == -1)
        return -1;
    return ((proj_id & 0xFF) << 24) | ((buf.st_ino & 0xFFFF) << 8) | (buf.st_dev & 0xFF);
}