#include <kos.h>
#include "../../core/hgl.h"

int hgl_fsize(const char *fileName) {
    file_t f;
    int size = -1;
    f = fs_open(fileName, O_RDONLY);
    if (f) {
        size = fs_total(f);
        fs_close(f);
    }
    return size;
}

void hgl_fread(void* buffer, int size, const char *fileName) {
    file_t f;
    f = fs_open(fileName, O_RDONLY);
    if (f) {
        fs_read(f, buffer, size);
        fs_close(f);
    }
}
