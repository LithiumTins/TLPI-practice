#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 1024
#define OPEN_FAILURE -10001
#define READ_FAILURE -10002
#define WRITE_FAILURE -10003
#define LSEEK_FAILURE -10004

int main(int argc, char *argv[])
{
    int rfd;
    int wfd;
    int read_num;
    int write_num;
    int write_index = 0;
    int zero_cnt = 0;
    char rbuf[BUF_SIZE];
    char wbuf[BUF_SIZE];
    int curr;

    rfd = open(argv[1], O_RDONLY, 
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (rfd == -1) exit(OPEN_FAILURE);
    wfd = open(argv[2], O_WRONLY,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (wfd == -1) exit(OPEN_FAILURE);

    while ((read_num = read(rfd, rbuf, BUF_SIZE)) > 0)
    {
        for (int i = 0; i < read_num; i++) 
        {
            if (rbuf[i] == '\0') 
            {
                if (write_index > 0)
                {
                    write_num = write(wfd, wbuf, write_index);
                    if (write_num == -1) exit(WRITE_FAILURE);
                    write_index = 0;
                }
                zero_cnt++;
            }
            else
            {
                if (zero_cnt > 0)
                {
                    curr = lseek(wfd, zero_cnt, SEEK_CUR);
                    if (curr == -1) exit(LSEEK_FAILURE);
                    zero_cnt = 0;
                }
                wbuf[write_index++] = rbuf[i];
            }
        }
    }

    if (read_num == -1) exit(READ_FAILURE);
    if (write_index > 0)
    {
        write_num = write(wfd, wbuf, write_index);
        if (write_num == -1) exit(WRITE_FAILURE);
    }

    return 0;
}