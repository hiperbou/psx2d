#include "ObjectPool.h"
#include "hgl_mem.h"
#include "stdio.h"
#include "hgl_types.h"



#define OBJECTPOOL_ITERATOR_START       \
    void * it = pool->objects;          \
    int maxLength = pool->maxLength;    \
    size_t chunkSize = pool->chunkSize; \
    for (int i=0; i < maxLength; i++) {

#define OBJECTPOOL_ITERATOR_END \
    it += chunkSize; }



inline static int* getPoolElementIDPtr(ObjectPool *pool, void *ptr) {
    return &getObjectPoolItemInfo(pool, ptr)->poolElementID;
}



inline static void setPoolElementID(ObjectPool *pool, void *ptr, int value) {
    *getPoolElementIDPtr(pool, ptr) = value;
}

inline static void storePoolElementID(ObjectPool *pool, PoolElement *elem) {
    *getPoolElementIDPtr(pool, elem->data) = elem->id;
}

static inline void initializeUnused(ObjectPool* pool) {
    OBJECTPOOL_ITERATOR_START
    setPoolElementID(pool, it, POOL_ITEM_UNUSED_ID);
    OBJECTPOOL_ITERATOR_END
}

ObjectPool* new_ObjectPool(int numObjects, int objectSize) {
    ObjectPool* pool = HGL_malloc(sizeof(ObjectPool));

    int chunkSize = objectSize + sizeof (ObjectPoolItemInfo);

    pool->maxLength = numObjects;
    pool->objectSize = objectSize;
    pool->chunkSize = chunkSize;
    pool->numObjectsAllocated = 0;
    pool->objects = HGL_malloc(numObjects * chunkSize);
    pool->elements = HGL_malloc(numObjects * sizeof(PoolElement));

    HGL_POOL_createPool(&pool->pool,
                        numObjects,
                        pool->objects,
                        chunkSize,
                        pool->elements);

    initializeUnused(pool);

    return pool;
}

void delete_ObjectPool(ObjectPool*pool) {
    HGL_free(pool->objects);
    HGL_free(pool->elements);
    HGL_free(pool);
}



PoolElement *ObjectPool_get(ObjectPool*pool) {
    PoolElement* elem = HGL_POOL_get(&pool->pool);
    storePoolElementID(pool, elem);
    pool->numObjectsAllocated = MIN(pool->numObjectsAllocated + 1, pool->maxLength);
    return elem;
}

void ObjectPool_free(ObjectPool*pool, void* chunk) {
    int * id = getPoolElementIDPtr(pool, chunk);
    HGL_POOL_freeByID(&pool->pool, *id);
    *id = POOL_ITEM_UNUSED_ID;
    pool->numObjectsAllocated = MIN(pool->numObjectsAllocated - 1, pool->maxLength);
}


#define OBJECTPOOL_ITERATOR_START       \
    void * it = pool->objects;          \
    int maxLength = pool->maxLength;    \
    size_t chunkSize = pool->chunkSize; \
    for (int i=0; i < maxLength; i++) {

#define OBJECTPOOL_ITERATOR_END \
    it += chunkSize; }

typedef void PoolIteratorCallback(void* item, int index);
static inline void ObjectPool_iterate(ObjectPool*pool, PoolIteratorCallback iteratorCallback) {
    OBJECTPOOL_ITERATOR_START
        iteratorCallback(it, i);
    OBJECTPOOL_ITERATOR_END
}

void ObjectPool_initialize(ObjectPool*pool, void * initializer, size_t initializerSize) {
    OBJECTPOOL_ITERATOR_START
        memcpy(it, initializer, initializerSize);
    OBJECTPOOL_ITERATOR_END
    //ObjectPool_iterate(pool, initializerCallback);
}

void ObjectPool_initialize_indexed(ObjectPool*pool, PoolInitializerCallback initializerCallback) {
    ObjectPool_iterate(pool, initializerCallback);
}

#undef OBJECTPOOL_ITERATOR_START
#undef OBJECTPOOL_ITERATOR_END