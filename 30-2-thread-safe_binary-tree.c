#define _XOPEN_SOURCE 500

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "tlpi_hdr.h"

struct node
{
    void *value;
    char *key;
    struct node *left, *right;
    pthread_mutex_t lock;
};

typedef struct node * tree;

void initialize(tree t)
{
    t = malloc(sizeof(struct node));
    t->key = "";
    t->left = t->right = NULL;
    t->value = NULL;
    pthread_mutex_init(&t->lock, NULL);
}

void add(tree t, char *key, void *value)
{
    struct node *tmp = malloc(sizeof (struct node));
    tmp->key = strdup(key);
    tmp->value = value;
    tmp->left = tmp->right = NULL;
    pthread_mutex_init(&tmp->lock, NULL);

    while (1)
    {
        pthread_mutex_t *lock = &t->lock;
        pthread_mutex_lock(lock);   // mutex zone start

        if (strcmp(t->key, key) < 0)
        {
            if (t->left)
                t = t->left;
            else
            {
                t->left = tmp;
                break;
            }
        }
        else
        {
            if (t->right)
                t = t->right;
            else
            {
                t->right = tmp;
                break;
            }
        }

        pthread_mutex_unlock(lock); // mutex zone end
    }
}

void delete(tree t, char *key)
{
    struct node *pre, *cur;
    pre = t;
    pthread_mutex_lock(&pre->lock);
    cur = pre->right;

    while (1)
    {
        if (!cur) return;

        pthread_mutex_lock(&cur->lock);   // mutex zone start;

        int cmp = strcmp(cur->key, key);
        if (cmp == 0)
        {
            
        }

    }
}
