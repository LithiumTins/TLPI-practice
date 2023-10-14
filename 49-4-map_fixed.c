#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    char *filename = "";

    int fd = open(filename, O_RDWR);
    int ps = sysconf(_SC_PAGESIZE);
    void *addr = mmap(NULL, 3 * ps, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    mmap(addr, ps, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
    mmap(addr + 2 * ps, ps, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 2 * ps);
    return 0;
}