#ifndef _PFS_ALLOC_H
#define _PFS_ALLOC_H

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

// 1G
#define DEFAULT_MEM_SIZE ((4L*1024L*1024L*1024))
//#define BLOCK_SIZE (sizeof(mem_block)+65536)
#define BLOCK_SIZE (4096+65536)

enum {
    ALLOC_RES_MEM, // allocated from resident memory(pre-allocated)
    ALLOC_HEAP_MEM    // allocated from heap
};

typedef struct mem_block mem_block;

struct mem_block {
    int alloc_flag;
    int block_size;
    mem_block* next;
    char data[0];
};

extern mem_block* freehead;

void init_pfs_alloc();
void* easy_pool_pfs_realloc(void *ptr, size_t size);


#endif
