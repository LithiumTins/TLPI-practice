#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include "tlpi_hdr.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

struct line
{
    char *buf;
    int size;
};
struct line* lines;
char buffer[4096];

int main(int argc, char *argv[])
{
    int line_num = 10;
    int opt;
    while ((opt = getopt(argc, argv, "n:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            line_num = atoi(optarg);
            break;
        default:
            usageErr("%s [-n num] file\n", argv[0]);
        }
    }

    if (optind != argc - 1)
        usageErr("%s [-n num] file\n", argv[0]);

    lines = (struct line *)malloc(line_num * sizeof(struct line));
    for (int i = 0; i < line_num; i++)
    {
        lines[i].size = 512;
        lines[i].buf = (char *)malloc(lines[i].size);
    }

    int fd = open(argv[optind], O_RDONLY);
    if (fd == -1)
        errExit("open");

    int line_count = 0;
    int read_num;
    int i = 0;
    int j = 0;
    while (true)
    {
        read_num = read(fd, buffer, 4096);
        if (read_num == -1)
            errExit("read");
        else if (read_num == 0)
            break;
        
        i = 0;
        while (i < read_num)
        {
            int ind = line_count % line_num;

            if (buffer[i++] == '\n')
            {
                lines[ind].buf[j] = '\0';
                line_count++;
                j = 0;
            }
            else
                lines[ind].buf[j++] = buffer[i - 1];
            
            if (j == lines[ind].size)
            {
                lines[ind].size *= 2;
                if ((lines[ind].buf = realloc(lines[ind].buf, lines[ind].size)) == NULL)
                    errExit("realloc");
            }
        }
    }

    if (j != 0) 
        lines[line_count++ % line_num].buf[j] = '\0';

    if (line_count < line_num) 
        printf("Only %d lines\n", line_count);
    for (int i = MAX(0, line_count - line_num + 1); i < line_count; i++)
        printf("%s\n", lines[i % line_num].buf);

    for (int i = 0; i < line_num; i++)
        free(lines[i].buf);
    free(lines);

    return 0;
}