#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <errno.h>

ssize_t my_sendfile(int out_fd, int in_fd, off_t *offset, size_t count)
{
    off_t origin;
    if ((origin = lseek(in_fd, 0, SEEK_CUR)) == -1)
        return -1;

    if (lseek(in_fd, offset, SEEK_SET) == -1)
        return -1;
    
    ssize_t read_num;
    char buf[count];
    if ((read_num = read(in_fd, buf, count)) == -1)
        return -1;

    ssize_t send_num, send_total = 0;
    while (1)
    {
        send_num = write(out_fd, buf + send_total, read_num - send_total);
        if (send_num == -1)
        {
            if (errno == EINTR)
                continue;
            else
                return -1;
        }
        
        send_total += send_num;
    }
    
    return send_num;
}