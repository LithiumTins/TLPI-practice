#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    if (argc <= 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file\n", argv[0]);

    for (int i = 1; i < argc; i++) 
    {
        struct stat st;
        if (stat(argv[i], &st) == -1)
            errExit("stat");

        mode_t mode = st.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
        mode = mode | S_IRUSR | S_IRGRP | S_IROTH;
        if (S_ISDIR(st.st_mode))
            mode = mode | S_IXUSR | S_IXGRP | S_IXOTH;
        
        if (chmod(argv[i], mode) == -1)
            errExit("chmod");
    }

    return 0;
}