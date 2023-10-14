#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define BUF_SIZE 4096

struct buf_node
{
    int fd;
    int cur;
    int size;
    char *buf;
    struct buf_node *next;
};

struct buf_node header_node = {0, 0, 0, NULL, NULL};
struct buf_node * header = &header_node;

ssize_t readLine(int fd, void *buffer, size_t n)
{
    struct buf_node *buf, *pre;
    char *c_buf = (char *)buffer;

    for (pre = header, buf = pre->next; buf; buf = buf->next, pre = pre->next)
        if (buf->fd == fd)
            break;

    if (!buf)
    {
        buf = malloc(sizeof(struct buf_node));
        buf->fd = fd;
        buf->buf = malloc(BUF_SIZE);
        while ((buf->size = read(fd, buf->buf, BUF_SIZE)) == -1 && errno == EINTR) ;
        if (buf->size <= 0)
        {
            free(buf->buf);
            free(buf);
            return -1;
        }
        buf->cur = 0;
        buf->next = header->next;
        header->next = buf;
    }

    int i;
    for (i = 0; i < n; i++)
    {
        char ch = buf->buf[buf->cur + i];
        
        if (buf->cur + i + 1 >= buf->size)
        {
            if ((buf->size = read(fd, buf->buf, BUF_SIZE)) <= 0)
            {
                pre->next = buf->next;
                free(buf->buf);
                free(buf);
            }
            else
                buf->cur = 0;
        }

        if (ch == '\n')
            break;
        
        c_buf[i] = ch;
    }

    return i;
}