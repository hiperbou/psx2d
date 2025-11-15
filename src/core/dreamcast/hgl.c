#include "../../core/hgl.h"
#include "../../platform/dreamcast/system.h"

void hgl_init() {
    initDisplay();
    initCD();
    initMemory();
    initFont();
}
