#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "pfs_alloc.h"
#include <pthread.h>

char *mem_str[] = {
   "ALLOC_RES_MEM",
   "ALLOC_HEAP_MEM"
};

static pthread_mutex_t mem_mutex;
mem_block* freehead = NULL;

void init_pfs_alloc() {


    uint64_t nblocks = DEFAULT_MEM_SIZE / BLOCK_SIZE;

    freehead=(mem_block*)malloc(nblocks*BLOCK_SIZE);

    uint64_t i;
    for (i=0; i<nblocks-1; ++i) {
        freehead[i].next= (mem_block*)((char*)&freehead[i]+BLOCK_SIZE);
        freehead[i].alloc_flag=ALLOC_RES_MEM;
        //printf("freehead[%d]=%p\n",i, freehead[i].next);
    }

    freehead[nblocks-1].next=NULL;

    pthread_mutex_init(&mem_mutex, NULL);
}

void* block_malloc(size_t size);
void  block_free(void *ptr);

void* easy_pool_pfs_realloc(void *ptr, size_t size) {
    static uint64_t msize=0;
    void*p;
    if (size) {
        p=block_malloc(size);

        return p;
    } else if (ptr) {
        block_free(ptr);
    }
}



void* block_malloc(size_t size) {
    void* ptr, *result;
    mem_block* mb;
    if (size <= 8192 || size>BLOCK_SIZE) {
        ptr=malloc(sizeof(mem_block)+size);
        mb=(mem_block*)ptr;
        mb->block_size=(size+sizeof(mem_block));
        mb->alloc_flag=ALLOC_HEAP_MEM;
        result=mb->data;
    } else {
        pthread_mutex_lock(&mem_mutex);
        if (freehead) {
            mb=freehead;
            freehead=freehead->next;
           // mb->block_size=size;
           //printf("mb=%p\n", mb);
           //printf("next=%p\n", freehead);
            mb->block_size=(size+sizeof(mem_block));
            mb->alloc_flag=ALLOC_RES_MEM;
            result=mb->data;
        } else {
            ptr=malloc(BLOCK_SIZE);
            mb=(mem_block*)ptr;
            //mb->block_size=size;
            mb->block_size=(size+sizeof(mem_block));
            mb->alloc_flag=ALLOC_HEAP_MEM;
            result=mb->data;
        }
        pthread_mutex_unlock(&mem_mutex);
    }

    if (mb&&mb->alloc_flag==ALLOC_RES_MEM) {
    //   printf("malloc flag=%s, block_size=%d, mb=%p\n", mem_str[mb->alloc_flag], mb->block_size,mb);
    }
    return result;
}

void block_free(void *ptr) {
    mem_block* mb;
    if (ptr) {
       mb=(mem_block*)((char*)ptr-sizeof(mem_block));
       if (mb&&mb->alloc_flag==ALLOC_RES_MEM) {
           //printf("free flag=%s, block_size=%d, mb=%p\n", mem_str[mb->alloc_flag], mb->block_size,mb);
        }

       if (mb->alloc_flag==ALLOC_RES_MEM) {
        pthread_mutex_lock(&mem_mutex);
           mb->next = freehead;
           freehead=mb;
        pthread_mutex_unlock(&mem_mutex);
       }else {
          free((void*)mb);
          mb=NULL;
       }
    }
}





