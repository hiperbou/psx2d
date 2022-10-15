#include <stddef.h>
#include "hgl_pool.h"

void HGL_POOL_createPool(Pool *pool, int maxLength, void *data, int sizeData, PoolElement* elements){
	pool->maxLength = maxLength;
	pool->data = data;
	pool->newPos = 0;
	pool->size = 0;
	pool->sizeData = sizeData;
	pool->elements = elements;
	int i=0;
	for(i=0; i<maxLength; i++) {
		pool->elements[i].data = NULL;
		pool->elements[i].id = 0;
	}
}

PoolElement * HGL_POOL_get(Pool *pool) {
	while(pool->elements[pool->newPos].data != NULL && pool->newPos < pool->maxLength){
	    pool->newPos++;
	}
	if(pool->newPos >= pool->maxLength) pool->newPos = pool->maxLength-1;
	if(pool->newPos >= pool->size) {
		pool->size = pool->newPos + 1;
	}
	void * data = ((char*)pool->data) + pool->newPos * pool->sizeData;
	PoolElement *element = &pool->elements[pool->newPos];
	element->id = pool->newPos;
	element->data = data;
	return element;
}

void HGL_POOL_free(Pool *pool, PoolElement *elem) {
	if(elem->id == pool->size - 1) pool->size--;
    if(elem->id < pool->newPos){
        pool->newPos = elem->id;
    }
    elem->id = 0;
    elem->data = NULL;
    //TODO: change pool->size accordingly?
}
