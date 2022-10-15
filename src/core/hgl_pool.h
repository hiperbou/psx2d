#ifndef __HGL_POOL_H___
#define __HGL_POOL_H___

typedef struct {
	int id;
	void * data;
}PoolElement;

typedef struct {
    int maxLength;
    void * data;
    int sizeData;
    int newPos;
    int size;
    PoolElement *elements;
}Pool;

void HGL_POOL_createPool(Pool *pool, int maxLength, void *data, int sizeData, PoolElement* elements);
PoolElement * HGL_POOL_get(Pool *pool);
void HGL_POOL_free(Pool *pool, PoolElement *elem);

#endif /* __HGL_POOL_H___ */
