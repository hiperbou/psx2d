#include "../../core/hgl.h"
#include <kos.h>
#include <dc/pvr.h>

void hgl_main_loop(void (*user_update)(void), void (*user_draw)(void)) {
    for (;;) {
        pvr_scene_begin();
        user_update();
        user_draw();
        pvr_scene_finish();
    }
}
