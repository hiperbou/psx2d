#include "../../core/hgl.h"
#include <kos.h>
#include <arch/gldc.h>

void hgl_main_loop(void (*user_update)(void), void (*user_draw)(void)) {
    for (;;) {
        user_update();
        user_draw();
        gldc_mini_wait_vbl();
    }
}
