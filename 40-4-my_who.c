#include <utmpx.h>
#include <unistd.h>
#include <time.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    char *tty = ttyname(STDIN_FILENO);
    if (tty == NULL)
        errExit("ttyname");

    struct utmpx ut, *rec;
    strncpy(ut.ut_line, tty, sizeof(ut.ut_line));
    rec = getutxline(&ut);
    if (rec == NULL)
        errExit("getutxline");
    
    printf("%s %s %s\n", rec->ut_user, rec->ut_line, 
            ctime((time_t *)&rec->ut_tv.tv_sec));

    return 0;
}