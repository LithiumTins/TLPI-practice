#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "tlpi_hdr.h"

#define CHUNK_SIZE (1 << 12)
#define WSIZE 4
#define DSIZE 8

#define PACK(size, alloc) ((size) | (alloc))
#define PUT(p, val) (*(unsigned int *)(p) = (val))
#define GET_SIZE(p) (*(unsigned int *)(p) & ~0x7)
#define GET_ALLOC(p) (*(unsigned int *)(p) & 0x1)
#define HDRP(p) ((char *)(p) - WSIZE)
#define FTRP(p) ((char *)(p) + GET_SIZE(HDRP(p)) - DSIZE)
#define NEXT_BLKP(p) ((char *)(p) + GET_SIZE(((char *)(p) - WSIZE)))
#define PREV_BLKP(p) ((char *)(p) - GET_SIZE(((char *)(p) - DSIZE)))

char * free_list = NULL;
char * list_end = NULL;

// sbrk wrapper
char * Sbrk(intptr_t increment)
{
    void * former_brk = sbrk(increment);
    if (former_brk == -1) 
    {
        errExit("sbrk");
    }
    return former_brk;
}

// initialize the free_list
// add a dummyhead to the free_list, size is 8, and allocated
// add a guardian end to the free_list, size is 0, and allocated
void mm_init(void)
{
    free_list = Sbrk(CHUNK_SIZE) + WSIZE;
    list_end = free_list + CHUNK_SIZE;
    PUT(free_list, PACK(DSIZE, 1));
    free_list += WSIZE;
    PUT(free_list, PACK(DSIZE, 1));
    PUT(HDRP(NEXT_BLKP(free_list)), PACK(CHUNK_SIZE - 16, 1));
    PUT(FTRP(NEXT_BLKP(free_list)), PACK(CHUNK_SIZE - 16, 1));
    PUT(HDRP(list_end), PACK(0, 1));
}

char * new_chunk(void)
{
    Sbrk(CHUNK_SIZE);
    if (GET_ALLOC(HDRP(list_end)))
    {
        PUT(HDRP(list_end), PACK(CHUNK_SIZE, 0));
        PUT(FTRP(list_end), PACK(CHUNK_SIZE, 0));
        list_end = NEXT_BLKP(list_end);
    }
    else
    {
        unsigned int block_size = CHUNK_SIZE + GET_SIZE(HDRP(PREV_BLKP(list_end)));
        char * pre_blk = PREV_BLKP(list_end);
        PUT(HDRP(pre_blk), PACK(block_size, 0));
        PUT(FTRP(pre_blk), PACK(block_size, 0));
        list_end = NEXT_BLKP(pre_blk);
    }
    PUT(HDRP(list_end), PACK(0, 1));
    return PREV_BLKP(list_end);
}

char * first_fit(size_t size)
{
    char * p = NEXT_BLKP(free_list);
    while (p != list_end)
    {
        if (GET_ALLOC(HDRP(p)) && GET_SIZE(HDRP(p)) >= size) return p;
        p = NEXT_BLKP(p);
    }
    p = new_chunk();
    return p;
}

void * my_malloc(size_t size)
{
    if (free_list == NULL) mm_init();

    if (size == 0) return NULL;

    size_t real_size = DSIZE;
    if (size <= DSIZE) real_size += ((size + (DSIZE - 1)) / DSIZE) * DSIZE;

    char * blk = first_fit(real_size);
    size_t blk_size = GET_SIZE(HDRP(blk));
    if (GET_SIZE(HDRP(blk)) > real_size)
    {
        PUT(HDRP(blk), PACK(real_size, 1));
        PUT(FTRP(blk), PACK(real_size, 1));
        char * next = NEXT_BLKP(blk);
        PUT(HDRP(next), PACK(blk_size - real_size, 0));
        PUT(FTRP(next), PACK(blk_size - real_size, 0));
    }
    return blk;
}

void my_free(void * ptr)
{
    char * prev = PREV_BLKP(ptr);
    char * next = NEXT_BLKP(ptr);
    char * blk = ptr;
    size_t size = GET_SIZE(ptr);
    
    if (!GET_ALLOC(HDRP(ptr)))
    {
        blk = prev;
        size += GET_SIZE(HDRP(prev));
    }
    if (!GET_ALLOC(HDRP(ptr)))
    {
        size += GET_SIZE(HDRP(next));
    }

    PUT(HDRP(blk), PACK(size, 0));
    PUT(FTRP(blk), PACK(size, 0));
}