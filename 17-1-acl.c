#include <acl/libacl.h>
#include <sys/acl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include "tlpi_hdr.h"

struct usr_node
{
    uid_t uid;
    int perm;
    struct usr_node *next;
}

struct grp_node
{
    gid_t gid;
    int perm;
    struct grp_node *next;
}

int get_perm(acl_entry_t entry)
{
    acl_permset_t permset;
    if (acl_get_permset(entry, &permset) == -1)
        errExit("acl_get_permset");

    int permVal;
    int perm = 0;
    if ((permVal = acl_get_perm(permset, ACL_READ)) == -1)
        errExit("acl_get_perm - ACL_READ");
    perm += 0x4 * permVal;
    if ((permVal = acl_get_perm(permset, ACL_WRITE)) == -1)
        errExit("acl_get_perm - ACL_WRITE");
    perm += 0x2 * permVal;
    if ((permVal = acl_get_perm(permset, ACL_EXECUTE)) == -1)
        errExit("acl_get_perm - ACL_EXECUTE");
    perm += 0x1 * permVal;

    return perm;
}

int main(int argc, char *argv[])
{
    if (argc != 4)
        usageErr("%s u/g uid/gid/uname/gname file\n", argv[0]);
    
    if (strlen(argv[1]) != 1)
        usageErr("%s u/g uid/gid/uname/gname file\n", argv[0]);

    // may lack declaration
    uid_t uid = 0;
    gid_t gid = 0;
    char *uname;
    char *gname;
    switch (argv[1][0])
    {
    case 'u':
        uid = atoi(argv[2]);
        uname = argv[2];
        if (uid)
        {
            struct passwd *pw = getpwuid(uid);
            if (!pw)
                errExit("getpwuid");
            uname = malloc(strlen(pw->pw_name) + 1);
            if (uname == NULL)
                errExit("malloc");
            strcpy(uname, pw->pw_name);
        }
        else
        {
            struct passwd *pw = getpwnam(uname);
            if (!pw)
                errExit("getpwnam");
            uid = pw->pw_uid;
        }
        break;
    case 'g':
        gid = atoi(argv[2]);
        gname = argv[2];
        if (gid)
        {
            struct group *grp = getgrgid(gid);
            if (!grp)
                errExit("getgrgid");
            gname = malloc(strlen(grp->gr_name) + 1);
            strcpy(gname, grp->gr_name);
        }
        else
        {
            struct group *grp = getgrnam(gname);
            if (!grp)
                errExit("getgrnam");
            gid = grp->gr_gid;
        }
        break;
    default:
        usageErr("%s u/g uid/gid/uname/gname file\n", argv[0]);
        break;
    }

    struct usr_node user_obj;
    struct usr_node *usr_list;
    struct grp_node group_obj;
    struct grp_node *grp_list;
    int mask = 7;
    int other = -1;

    if ((usr_list = malloc(sizeof(struct usr_node))) == -1)
        errExit("malloc");
    if ((grp_list = malloc(sizeof(struct grp_node))) == -1)
        errExit("malloc");
    usr_list->next = NULL;
    grp_list->next = NULL;

    struct stat st;
    if (stat(argv[3], &st) == -1)
        errExit("stat");
    user_obj.uid = st.st_uid;
    group_obj.gid = st.st_gid;

    acl_t acl = acl_get_file(argv[3], ACL_TYPE_ACCESS);
    if (acl == -1)
        errExit("acl_get_file");

    acl_entry_t entry;
    int res;
    for (
        int entryId = ACL_FIRST_ACCESS;
        res = acl_get_entry(acl, &entry); 
        entryId = ACL_NEXT_ACCESS
    ) 
    {
        if (res == -1)
            errExit("acl_get_entry");
        
        acl_tag_t tag;
        if (acl_get_tag_type(entry, &tag) == -1)
            errExit("acl_get_tag_type");
        
        int ent_perm = get_perm(entry);
        
        switch (tag)
        {
        case ACL_USER_OBJ:
            user_obj.per = ent_perm;
            break;
        case ACL_USER:
            struct usr_node *tmp = malloc(sizeof(struct usr_node));
            if (tmp == NULL)
                errExit("malloc");
            uid_t *uidp = acl_get_qualifier(entry);
            if (uidp == NULL)
                errExit("acl_get_qualifier");
            tmp->uid = *uidp;
            tmp->perm = ent_perm;
            tmp->next = usr_list->next;
            usr_list->next = tmp;
            break;
        case ACL_GROUP_OBJ:
            group_obj.perm = ent_perm;
            break;
        case ACL_GROUP:
            struct grp_node *tmp = malloc(sizeof(struct grp_node));
            if (tmp == NULL)
                errExit("malloc");
            gid_t *gidp = acl_get_qualifier(entry);
            if (gidp == NULL)
                errExit("acl_get_qualifier");
            tmp->gid = *gidp;
            tmp->perm = ent_perm;
            tmp->next = grp_list->next;
            grp_list->next = tmp;
            break;
        case ACL_MASK:
            mask = ent_perm;
            break;
        default:
            other = ent_perm;
            break;
        }
    }
    
    struct grp_node *grp_to_check;
    grp_to_check = malloc(sizeof(struct grp_node));
    if (grp_to_check == NULL)
        errExit("malloc");
    grp_to_check->next = NULL;

    if (argv[1][0] == 'u')
    {
        int match = 0;
        if (uid == user_obj.uid)
        {
            printf("%-12s         %c%c%c\n", "user_obj",
                (user_obj.perm & 0x4) ? 'r' : '-',
                (user_obj.perm & 0x2) ? 'w' : '-',
                (user_obj.perm & 0x1) ? 'x' : '-');
            match = 1;
        }
        else
        {
            struct usr_node *p = usr_list->next;
            while (p)
            {
                if (p->uid == uid)
                {
                    int final_perm = p->perm & mask;
                    printf("%-12s%-8d %c%c%c\n", "user", uid,
                        (final_perm & 0x4) ? 'r' : '-',
                        (final_perm & 0x2) ? 'w' : '-',
                        (final_perm & 0x1) ? 'x' : '-');
                    match = 1;
                    break;
                }
            }
        }

        if (!match)
        {
            struct group *ent;
            errno = 0;
            while ((ent = getgrent()) != NULL)
            {
                int found = 0;
                for (char *mem = *(grp_to_check->gr_mem); mem != NULL; mem++)
                {
                    if (strcmp(uname, mem) == 0)
                    {
                        struct grp_node *tmp = malloc(sizeof(struct group));
                        if (tmp == NULL)
                            errExit("malloc");
                        tmp->gid = ent->gr_gid;
                        tmp->next = grp_to_check->next;
                        grp_to_check->next = tmp->next;
                    }
                }
            }
            if (errno != 0)
                errExit("getgrent");
        }
    }
    else if (argv[1][0] == 'g')
    {
        struct grp_node *tmp = malloc(sizeof(struct grp_node));
        if (tmp == NULL)
            errExit("malloc");
        tmp->gid = gid;
        tmp->next = grp_to_check->next;
        grp_to_check->next = tmp;
    }

    int match = 0;
    for (struct grp_node *p = grp_to_check->next; p != NULL && !match; p = p->next)
    {
        for (struct grp_node *acl_p = grp_list->next; acl_p != NULL && !match; acl_p = acl_p->next)
        {
            if (p->gid == acl_p->gid)
            {
                int final_perm = acl_p->perm & mask;
                printf("%-12s%-8d %c%c%c\n", "group", uid,
                    (final_perm & 0x4) ? 'r' : '-',
                    (final_perm & 0x2) ? 'w' : '-',
                    (final_perm & 0x1) ? 'x' : '-');
                match = 1;
                break;
            }
        }
    }

    if (!match)
    {
        printf("%-12s         %c%c%c\n", "other",
            (other & 0x4) ? 'r' : '-',
            (other & 0x2) ? 'w' : '-',
            (other & 0x1) ? 'x' : '-');
    }

    for (struct usr_node *u_tmp = usr_list; u_tmp != NULL; u_tmp = usr_list)
    {
        usr_list = usr_list->next;
        free(u_tmp);
    }
    for (struct grp_node *g_tmp = grp_list; g_tmp != NULL; g_tmp = grp_list)
    {
        grp_list = grp_list->next;
        free(g_tmp);
    }
    for (struct grp_node *g_tmp = grp_to_check; g_tmp != NULL; g_tmp = grp_to_check)
    {
        grp_to_check = grp_to_check->next;
        free(g_tmp);
    }

    return 0;
}