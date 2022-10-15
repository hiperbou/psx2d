#ifndef __HGL_OBJECTPOOL_H__
#define __HGL_OBJECTPOOL_H__

#include <stddef.h>
#include "hgl_pool.h"

typedef struct{
    int maxLength;
    size_t objectSize;
    size_t chunkSize;
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

#endif //__HGL_OBJECTPOOL_H__
