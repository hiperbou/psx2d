#include "system.h"
#include <kos.h>
#include <kos/thread.h>
#include <dc/biosfont.h>
#include <dc/flashrom.h>
#include <dc/maple.h>
#include <dc/maple/controller.h>

KOS_INIT_FLAGS(INIT_DEFAULT);

#define USE_50HZ 0
#define USE_60HZ 1

static int show_pal_menu(void) {
    maple_device_t *cont;
    cont_state_t *state;

    vid_set_mode(DM_640x480_PAL_IL, PM_RGB565);

    bfont_draw_str(vram_s + 640 * 200 + 64, 640, 1, "Press A to run at 60Hz");
    bfont_draw_str(vram_s + 640 * 240 + 64, 640, 1, "or B to run at 50Hz");

    for(;;) {
        for (int i = 0; i < 4; i++) {
            cont = maple_enum_type(i, MAPLE_FUNC_CONTROLLER);
            if (cont) {
                state = (cont_state_t *)maple_dev_status(cont);
                if (state) {
                    if (state->buttons & CONT_A)
                        return USE_60HZ;
                    if (state->buttons & CONT_B)
                        return USE_50HZ;
                }
            }
        }
        thd_sleep(20);
    }
}

void initDisplay() {
    int cable_type = vid_check_cable();
    int mode = DM_640x480_NTSC_IL;

    if (cable_type == CT_VGA) {
        mode = DM_640x480_VGA;
    } else {
        int region = flashrom_get_region();
        if (region == FLASHROM_REGION_EUROPE) {
            if (show_pal_menu() == USE_50HZ) {
                mode = DM_640x480_PAL_IL;
            }
        }
    }

    vid_init(mode, PM_RGB565);
    pvr_init_defaults();
}

void display() {
    pvr_wait_ready();
    pvr_list_begin(PVR_LIST_OP_POLY);
}

void initFont() {
    bfont_set_encoding(BFONT_CODE_ISO8859_1);
}

pvr_ptr_t load_texture(const uint8_t *data, uint32_t width, uint32_t height) {
    pvr_ptr_t texture = pvr_mem_alloc(width * height * 2);
    pvr_txr_load((void*)data, texture, width * height * 2);
    return texture;
}
