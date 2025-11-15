#include "../../core/hgl.h"
#include <malloc.h>

void* hgl_malloc(int size) {
    return malloc(size);
}

void hgl_free(void *ptr) {
    free(ptr);
}
