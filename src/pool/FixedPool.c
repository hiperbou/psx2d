#include "FixedPool.h"
#include "stdio.h"
#include "hgl_mem.h"

static void *allocateBlock(int mChunksPerBlock, int chunkSize) {
    int blockSize = mChunksPerBlock * chunkSize;

    void *blockBegin = HGL_malloc(blockSize);

    Chunk *chunk = (Chunk*)blockBegin;

    for (int i = 0; i < mChunksPerBlock - 1; ++i) {
        chunk->next = (Chunk *)(((char *)chunk) + chunkSize);
        //chunk->index = i;
        chunk = chunk->next;
    }

    chunk->next = NULL;

    return blockBegin;
}

FixedPool* new_FixedPool(int chunksPerBlock, int chunkSize) {
    FixedPool* pool = HGL_malloc(sizeof(FixedPool));
    pool->block = allocateBlock(chunksPerBlock, chunkSize);
    pool->currentChunk = (Chunk*)pool->block;
    //pool->numAllocatedObjects = 0;
    pool->chunkSize = chunkSize;
    return pool;
}

void delete_FixedPool(FixedPool*pool) {
    HGL_free(pool->block);
    HGL_free(pool);
}

void *FixedPool_get(FixedPool*pool) {
    Chunk *freeChunk = pool->currentChunk;

    if (freeChunk->next == NULL) {
        //TODO: warning no more free chunks
        return pool->currentChunk;
    }

    pool->currentChunk = pool->currentChunk->next;
    //printf("FixedPool_get chunk %i\n", FixedPool_free->index);
    //pool->numAllocatedObjects++;
    return freeChunk;
}

void FixedPool_free(FixedPool*pool, void *chunk) {
    ((Chunk *)chunk)->next = pool->currentChunk;
    pool->currentChunk = (Chunk *)chunk;
    //pool->numAllocatedObjects--;
    //printf("free chunk %i\n", pool->currentChunk->index);
}


/*void forEachInPool(FixedPool*pool, int (*condition)(void *), void (*action)(void *)) {
    void * chunk = pool->block;
    int i = 0;
    int particlesToProcess = pool->numAllocatedObjects;
    int objectSize = pool->objectSize;
    while(i < MAX_NUM_PARTICLES && particlesToProcess>0) {
        if (condition(chunk)) {
            particlesToProcess--;
            action(chunk);
        }
        chunk = chunk + objectSize;
        i++;
    }
    printf("processed %i iterations %i\n", pool->numAllocatedObjects, i);
}*/