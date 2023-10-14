#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    if (mkdir("test_dir", S_IRWXU) == -1)
        errExit("mkdir");
    
    int fd = open("test_dir/test", O_WRONLY | O_CREAT, 
        S_IRWXU);
    if (fd == -1)
        errExit("open");
    close(fd);
    
    if (chmod("test_dir", S_IRUSR) == -1)
        errExit("chmod");
    
    return 0;
}