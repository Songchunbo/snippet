#include "pfs_alloc.h"


int main(int argc, char* argv[]) {
    init_pfs_alloc();

    void* buf1[1024*20];
    void* buf2[10];
    void* buf3[10];
    void* buf4[10];

    int i;
    ///printf("nblocks = %llu, nblocks*BLOCK_SIZE=%llu\n", nblocks, nblocks*BLOCK_SIZE);
    //printf("sizeof mem_block=%d\n", sizeof(mem_block));

    for (i=0; i<1024*20; ++i) {
        buf1[i]=easy_pool_pfs_realloc(buf1[i], 65536);
        //buf2[i]=block_malloc(75000);
        //buf3[i]=block_malloc(800);
    }

    for (i=0; i<1024*20; ++i) {
        easy_pool_pfs_realloc(buf1[i], 0);
        //block_free(buf1[i]);
    //    block_free(buf2[i]);
    //    block_free(buf3[i]);
    }

    return 0;
}
