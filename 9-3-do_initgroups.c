#include <unistd.h>
#include <string.h>
//#include <grp.h>

// forward declaration (to be deleted on linux)
#define NGROUPS_MAX 65536
typedef int gid_t;

struct group {
    char *gr_name;
    char *gr_passwd;
    gid_t gr_gid;
    char **gr_mem;
};

struct group *getgrnam(const char *name);
struct group *getgrgid(gid_t gid);
struct group *getgrent(void);
void setgrent(void);
void endgrent(void);
int setgroups(size_t gidsetsize, const gid_t *grouplist);
// end forward declaration

int initgroups(const char *user, gid_t group)
{
    struct group *gp;
    gid_t *grouplist = (gid_t *)malloc((NGROUPS_MAX + 1) * sizeof(gid_t));
    int cnt = 1;
    int res = 0;

    grouplist[0] = group;

    while ((gp = getgrent()) != NULL)
    {
        char **mem = gp->gr_mem;
        while (mem && strcmp(*mem, user) != 0) mem++;
        if (mem) grouplist[cnt++] = gp->gr_gid;
    }
    endgrent();

    res = setgroups(cnt, grouplist);
    free(grouplist);
    return res;
}