#define _XOPEN_SOURCE 500
#include "useful.h"
#include <ftw.h>

int dir = 0;
int link = 0;
int file = 0;

int counter(const char *fpath, const struct stat *sb, 
    int typeflag, struct FTW *ftwbuf)
{
    switch (typeflag)
    {
    case FTW_D:
    case FTW_DP:
    case FTW_DNR:
        dir++;
        break;
    case FTW_SL:
    case FTW_SLN:
        link++;
        break;
    case FTW_F:
        file++;
        break;
    default:
        printf("unknown type %d\n", typeflag);
        break;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s dir", argv[0]);

    if (nftw(argv[1], counter, 20, 0) == -1)
        errExit("nftw");
    return 0;
}