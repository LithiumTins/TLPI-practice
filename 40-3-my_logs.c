#define _GNU_SOURCE
#include <utmpx.h>
#include <paths.h>
#include <string.h>
#include "tlpi_hdr.h"

// The function login() takes the supplied struct utmp, ut, 
//     and writes it to both the utmp and the wtmp file.
void my_login(const struct utmpx *ut)
{
    pututxline(&ut);
    updwtmpx(_PATH_WTMP, &ut);
}

// The function logout() clears the entry in the utmp file again.
int my_logout(const char *ut_line)
{
    struct utmpx ut, *rec;

    memset(&ut, 0, sizeof(ut));
    strncpy(ut.ut_line, ut_line, 32);

    rec = getutxline(&ut);
    memcpy(&ut, rec, sizeof(struct utmpx));

    ut.ut_type = DEAD_PROCESS;
    time((time_t *)&ut.ut_tv.tv_sec);
    memset(&ut.ut_user, 0, sizeof(ut.ut_user));

    if (pututxline(&ut) == NULL)
        return -1;
    
    return 0;
}

void my_logwtmp(const char *line, const char *name, const char *host)
{
    struct utmpx ut;
    strncpy(ut.ut_line, line, sizeof(ut.ut_line));
    strncpy(ut.ut_user, name, sizeof(ut.ut_user));
    strncpy(ut.ut_host, host, sizeof(ut.ut_host));
    ut.ut_pid = getpid();
    time((time_t *)&ut.ut_tv.tv_sec);
    updwtmpx(_PATH_WTMP, &ut);
}