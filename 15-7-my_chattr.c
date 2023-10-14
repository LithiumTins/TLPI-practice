#include <sys/ioctl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
        usageErr("%s (+/-/=)flag file\n", argv[0]);

    switch (argv[2][0])
    {
    case '+':
    case '-':
    case '=':
        break;
    default:
        usageErr("%s (+/-/=)flag file\n", argv[0]);
    }

    int fd = open(argv[2], O_RDONLY);
    if (fd == -1)
        errExit("open");

    int attr = 0;
    if (argv[2][0] != '=' || 
        ioctl(fd, FS_IOC_GETFLAGS, &attr) == -1)
            errExit("ioctl getting flags");
    
    for (int i = 1; argv[1][0] != '\0'; i++)
    {
        int FLAG;
        switch (argv[1][i])
        {
        case 'a':
            FLAG = FS_APPEND_FL;
            break;
        case 'c':
            FLAG = FS_COMPR_FL;
            break;
        case 'D':
            FLAG = FS_DIRSYNC_FL;
            break;
        case 'i':
            FLAG = FS_IMMUTABLE_FL;
            break;
        case 'j':
            FLAG = FS_JOURNAL_DATA_FL;
            break;
        case 'A':
            FLAG = FS_NOATIME_FL;
            break;
        case 'd':
            FLAG = FS_NODUMP_FL;
            break;
        case 't':
            FLAG = FS_NOTAIL_FL;
            break;
        case 's':
            FLAG = FS_SECRM_FL;
            break;
        case 'S':
            FLAG = FS_SYNC_FL;
            break;
        case 'T':
            FLAG = FS_TOPDIR_FL;
            break;
        case 'u':
            FLAG = FS_UNRM_FL;
            break;
        default:
            break;
        }
        if (argv[1][0] == '=')
            attr &= FLAG;
        else
            attr |= FLAG;
    }

    if (ioctl(fd, FS_IOC_SETFLAGS, &attr) == -1)
        errExit("ioctl setting flags");

    return 0;
}