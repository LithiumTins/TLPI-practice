#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/xattr.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    if (argc != 6)
        usageErr("%s -n name -v value file\n", argv[0]);

    int opt;
    char *name;
    char *value;
    while ((opt = getopt(argc, argv, "n:v:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            name = malloc(strlen(optarg) + 1);
            strcpy(name, optarg);
            break;
        case 'v':
            value = malloc(strlen(optarg) + 1);
            strcpy(value, optarg);
        default:
            usageErr("%s -n name -v value file\n", argv[0]);
        }
    }

    if (optind + 1 != argc)
        usageErr("%s -n name -v value file\n", argv[0]);

    if (setxattr(argv[5], name, value, strlen(value) + 1, 0) == -1)
        errExit("setxattr");

    return 0;
}