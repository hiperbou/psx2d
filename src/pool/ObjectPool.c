#include "ObjectPool.h"
#include "hgl_mem.h"

ObjectPool* new_ObjectPool(int numObjects, int objectSize) {
    ObjectPool* pool = HGL_malloc(sizeof(ObjectPool));

    int chunkSize = objectSize + sizeof (ObjectPoolItemInfo);

    pool->maxLength = numObjects;
    pool->objectSize = objectSize;
    pool->chunkSize = chunkSize;
    pool->objects = HGL_malloc(numObjects * chunkSize);
    pool->elements = HGL_malloc(numObjects * sizeof(PoolElement));

    HGL_POOL_createPool(&pool->pool,
                        numObjects,
                        pool->objects,
                        chunkSize,
                        pool->elements);
    return pool;
}

void delete_ObjectPool(ObjectPool*pool) {
    HGL_free(pool->objects);
    HGL_free(pool->elements);
    HGL_free(pool);
}

inline static ObjectPoolItemInfo* getObjectPoolItemInfo(ObjectPool *pool, void *ptr) {
    return ((void *)ptr) + pool->objectSize;
}

inline static int* getPoolElementIDPtr(ObjectPool *pool, void *ptr) {
    return &getObjectPoolItemInfo(pool, ptr)->poolElementID;
}

inline static int getPoolElementId(ObjectPool *pool, void *ptr) {
    return getObjectPoolItemInfo(pool, ptr)->poolElementID;
}

inline static void storePoolElementId(ObjectPool *pool, PoolElement *elem) {
    *getPoolElementIDPtr(pool, elem->data) = elem->id;
}

PoolElement *ObjectPool_get(ObjectPool*pool) {
    PoolElement* elem = HGL_POOL_get(&pool->pool);
    storePoolElementId(pool, elem);
    return elem;
}

void ObjectPool_free(ObjectPool*pool, void* chunk) {
    HGL_POOL_freeByID(&pool->pool, getPoolElementId(pool, chunk));
}