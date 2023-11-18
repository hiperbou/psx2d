#include "system.h"
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

void HGL_frame() {
    drawTexts();
    display();
}

