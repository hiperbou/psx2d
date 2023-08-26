#include <stdint.h>
#include "hgl_mem.h"
#include "malloc.h"
#include "memory.h"
//#include "memory.h"

void HGL_InitHeap(unsigned long * buffer, unsigned long size) {
    InitHeap3(buffer, size);
}
void HGL_free(void *ptr) {
    free3(ptr);
}
void *HGL_malloc(size_t size) {
    return malloc3(size);
}
void *HGL_calloc(size_t numItems, size_t size) {
    return calloc3(numItems, size);
}
void *HGL_realloc(void *ptr, size_t size) {
    return realloc3(ptr, size);
}

void *HGL_memcpy(uint8_t *dest, const uint8_t *src, size_t size) {
   return memcpy(dest, src, size);
}

void *HGL_memset(uint8_t *dest, int value, size_t size) {
    return memset(dest, value, size);
}