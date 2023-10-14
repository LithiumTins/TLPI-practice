#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
//#include <pwd.h>

// forward declaration (to be deleted)
typedef int uid_t;
typedef int gid_t;
struct passwd
{
    char *pw_name;
    char *pw_passwd;
    uid_t pw_uid;
    gid_t pw_gid;
    char *pw_gecos;
    char *pw_dir;
    char *pw_shell;
};
struct passwd *getpwent(void);
void setpwent(void);
void endpwent(void);
// over

struct passwd *m_getpwnam(const char *name)
{
    static struct passwd * pwd;
    while ((pwd = getpwent()) != NULL)
    {
        if (strcmp(pwd->pw_name, name) == 0)
        {
            endpwent();
            return pwd;
        }
    }
}
