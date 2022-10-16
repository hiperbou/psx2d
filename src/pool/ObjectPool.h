#ifndef __HGL_OBJECTPOOL_H__
#define __HGL_OBJECTPOOL_H__

#include <stddef.h>
#include "hgl_pool.h"
#include "memory.h"

typedef struct{
    int maxLength;
    size_t objectSize;
    size_t chunkSize;
    int numObjectsAllocated;
    Pool pool;
    void* objects;
    PoolElement* elements;
}ObjectPool;

typedef struct {
    int poolElementID;
}ObjectPoolItemInfo;

ObjectPool* new_ObjectPool(int numObjects, int objectSize);
void delete_ObjectPool(ObjectPool*pool);
PoolElement *ObjectPool_get(ObjectPool*pool);
void ObjectPool_free(ObjectPool*pool, void* chunk);
void ObjectPool_initialize(ObjectPool*pool, void * initializer, size_t initializerSize);

typedef void PoolInitializerCallback(void* item, int index);
void ObjectPool_initialize_indexed(ObjectPool*pool, PoolInitializerCallback initializerCallback);

inline static ObjectPoolItemInfo* getObjectPoolItemInfo(ObjectPool *pool, void *ptr) {
    return ((void *)ptr) + pool->objectSize;
}

inline static int getPoolElementID(ObjectPool *pool, void *ptr) {
    return getObjectPoolItemInfo(pool, ptr)->poolElementID;
}

#define POOL_ITEM_UNUSED_ID (-1)

#define OBJECTPOOL_ITERATOR_ALLOCATED_START(POOL, TYPE) \
    int _i = 0;                              \
    int _entitiesToProcess = POOL->numObjectsAllocated;\
    void * _address = POOL->objects;          \
    TYPE * it = _address;          \
    int _maxLength = POOL->maxLength;    \
    size_t _chunkSize = POOL->chunkSize; \
    while (_i < _maxLength && _entitiesToProcess > 0) { \
        if (getPoolElementID(POOL, it) != POOL_ITEM_UNUSED_ID) \
        {                               \
            _entitiesToProcess--;

#define OBJECTPOOL_ITERATOR_ALLOCATED_END \
        }\
        _i++;                              \
        _address += _chunkSize;             \
        it = _address;                     \
    }

#define OBJECTPOOL_ITERATOR_IF_START(POOL, TYPE, CONDITION) \
            OBJECTPOOL_ITERATOR_ALLOCATED_START(POOL, TYPE, CONDITION)   \
                if(CONDITION) {

#define OBJECTPOOL_ITERATOR_IF_END \
                }                              \
            OBJECTPOOL_ITERATOR_ALLOCATED_END


#endif //__HGL_OBJECTPOOL_H__
