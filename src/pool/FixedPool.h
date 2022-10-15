
#ifndef __HGL_POOL_H__
#define __HGL_POOL_H__

typedef struct Chunk {
    void *next;
    //int index;
}Chunk;

typedef struct FixedPool {
    Chunk *currentChunk;
    void *block;
    //int numAllocatedObjects;
    int chunkSize;
}FixedPool;

FixedPool *new_FixedPool(int chunksPerBlock, int chunkSize);
void delete_FixedPool(FixedPool*);

void *FixedPool_get(FixedPool *pool);
void FixedPool_free(FixedPool *pool, void *chunk);

#endif //__HGL_POOL_H__
