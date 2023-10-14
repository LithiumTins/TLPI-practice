#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    int fd = open("test_file", O_RDONLY | O_CREAT | O_EXCL, 
        S_IRWXG | S_IRWXO);
    close(fd);
    int fd = open("test_file", O_RDONLY);
    printf("%d\n", fd);
    return 0;
}