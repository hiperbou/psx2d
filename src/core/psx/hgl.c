#include <platform/psx/system.h>
#include "input.h"
#include "sprites.h"
#include "fpg.h"
#include "../hgl.h"

void HGL_init() {
    initDisplay();
    initCD();
    initMemory();
    initFont();
    initInput();

    initSprites();
    init_particles();
    
    init_fpgs();

}

// New empty function
void HGL_start_frame(void) {
    // PSX screen clearing and vsync are typically handled by display() or GsSortCls()
    // For now, this function will be empty.
}

void HGL_frame() {
    drawTexts();
    display();
}

