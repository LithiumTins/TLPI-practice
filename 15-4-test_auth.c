#include <unistd.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file", argv[0]);
    
    if (euidaccess(argv[1], F_OK) == -1)
    {
        printf("There's no file \"%s\"", argv[1]);
        return 0;
    }

    printf("Have authority to ");

    if (euidaccess(argv[1], R_OK) == 0) printf("read ");
    if (euidaccess(argv[1], W_OK) == 0) printf("write ");
    if (euidaccess(argv[1], X_OK) == 0) printf("execute ");

    printf("file \"%s\"\n", argv[1]);

    return 0;
}