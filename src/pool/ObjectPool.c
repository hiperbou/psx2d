#include "ObjectPool.h"
#include "hgl_mem.h"

ObjectPool* new_ObjectPool(int numObjects, int objectSize) {
    ObjectPool* pool = HGL_malloc(sizeof(ObjectPool));
    pool->maxLength = numObjects;
    pool->objectSize = objectSize;
    pool->objects = HGL_malloc(numObjects * objectSize);
    pool->elements = HGL_malloc(numObjects * sizeof(PoolElement));

    HGL_POOL_createPool(&pool->pool,
                        numObjects,
                        pool->objects,
                        objectSize,
                        pool->elements);
    return pool;
}

void delete_ObjectPool(ObjectPool*pool) {
    HGL_free(pool->objects);
    HGL_free(pool->elements);
    HGL_free(pool);
}

PoolElement *ObjectPool_get(ObjectPool*pool) {
    return HGL_POOL_get(&pool->pool);
}

void ObjectPool_free(ObjectPool*pool, ObjectPoolItemID *chunk) {
    //printf("free %p id %i\n", chunk, chunk->id);
    HGL_POOL_free(&pool->pool, &pool->elements[chunk->id]);
}