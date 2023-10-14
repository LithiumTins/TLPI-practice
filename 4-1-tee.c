#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

int Open(const char *pathname, int flags, mode_t mode)
{
    int fd = open(pathname, flags, mode);
    if (fd == -1)
    {
        printf("Error at opening file\n");
        exit(-1);
    }
    return fd;
}

int main(int argc, char *argv[]) 
{
    int opt;
    int fds[10] = {};
    int cur = 0;
    int flags = O_WRONLY | O_CREAT;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    char buf[1024];
    int read_num;

    while ((opt = getopt(argc, argv, "+a:")) != -1) 
    {
        if (opt == '?')
        {
            printf("Unknown option: %c\n", optopt);
            exit(-1);
        }

        switch (opt)
        {
        case 'a':
            if (cur == 10)
            {
                printf("Too many files\n");
                exit(-1);
            }
            fds[cur++] = Open(optarg, flags | O_APPEND, mode);
            break;
        default:
            printf("Unknown opt in switch\n");
            exit(-1);
            break;
        }
    }

    if (optind < argc)
    {
        for (int i = optind; i < argc; i++)
        {
            if (cur == 10)
            {
                printf("Too many files\n");
                exit(-1);
            }
            else
            {
                fds[cur++] = Open(argv[i], flags | O_TRUNC, mode);
            }
        }
    }

    while ((read_num = read(STDIN_FILENO, buf, 1024)) > 0)
    {
        for (int i = 0; i < cur; i++)
        {
            write(fds[i], buf, read_num);
        }
    }

    if (read_num == -1)
    {
        printf("Error at reading\n");
        exit(-1);
    }
    else
    {
        for (int i = 0; i < cur; i++)
        {
            close(fds[i]);
        }
    }

    return 0;
}