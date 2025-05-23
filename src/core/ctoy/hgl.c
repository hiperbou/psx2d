//#include "system.h"
//#include "input.h"
#include "../../engine/sprites.h"
#include "../../media/fpg.h"
#include "../hgl.h" // Added for HGL_start_frame prototype

#include <ctoy.h>
//#include "../util/img_util.c"

struct m_image buffer = M_IMAGE_IDENTITY();

void initDisplay() {
    ctoy_window_title("Game!");
    ctoy_window_size(320 * 2, 240 * 2);
    //ctoy_window_size(960, 720);
    //ctoy_window_size(1280, 960);
    //m_image_create(&buffer, M_FLOAT, image.width, image.height, 3);
    m_image_create(&buffer, M_FLOAT, 320, 240, 3);
    setClearColor(0, 0, 0);
}

void HGL_init() {
    initDisplay();
    //initCD();
    //initMemory();
    //initFont();
    //initInput();

    initSprites();
    init_particles();
    
    init_fpgs();

}

void clear(struct m_image *dest); // Assume this is declared elsewhere or define it if missing and simple

void HGL_start_frame(void) {
    clear(&buffer);
}

void HGL_frame() {
    //drawTexts();
    //display();
    ctoy_swap_buffer(&buffer);
}

