#ifndef ___HGL_MEM_H___
#define ___HGL_MEM_H___

#include <stddef.h>
#include <stdint.h>

extern void HGL_InitHeap(unsigned long *buffer, unsigned long size);
extern void HGL_free(void *ptr);
extern void *HGL_malloc(size_t size);
extern void *HGL_calloc(size_t numItems, size_t size);
extern void *HGL_realloc(void *ptr, size_t size);
extern void *HGL_memcpy(uint8_t *dest, const uint8_t *src, size_t size);
extern void *HGL_memset(uint8_t *dest, int value, size_t size);

#endif //___HGL_MEM_H___
