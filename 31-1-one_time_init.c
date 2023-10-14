#include <pthread.h>
#include "tlpi_hdr.h"

struct control_t
{
    int inited;
    pthread_mutex_t lock;
};

void one_time_init(struct control_t *control, void (*init)(void))
{
    if (!control->inited)
    {
        pthread_mutex_lock(&control->lock);     // mutex zone start

        if (!control->inited) 
        {
            init();
            control->inited = 1;
        }

        pthread_mutex_unlock(&control->lock);   // mutex zone end
    }
}