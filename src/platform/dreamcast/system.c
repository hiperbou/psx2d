#include "system.h"
#include <kos.h>

#include <dc/flashrom.h>

KOS_INIT_FLAGS(INIT_DEFAULT);

void initDisplay() {
    int cable_type = vid_check_cable();
    if (cable_type == CT_VGA) {
        vid_init(DM_640x480_VGA, PM_RGB565);
    } else {
        int region = flashrom_get_region();
        if (region == FLASHROM_REGION_EUROPE) {
            vid_init(DM_640x480_PAL_IL, PM_RGB565);
        } else {
            vid_init(DM_640x480_NTSC_IL, PM_RGB565);
        }
    }
    pvr_init_defaults();
}

void display() {
    pvr_wait_ready();
    pvr_scene_begin();
    pvr_list_begin(PVR_LIST_OP_POLY);
    pvr_scene_finish();
}

void initFont() {
}

pvr_ptr_t load_texture(const uint8_t *data, uint32_t width, uint32_t height) {
    pvr_ptr_t texture = pvr_mem_alloc(width * height * 2);
    pvr_txr_load((void*)data, texture, width * height * 2);
    return texture;
}
