#include "system.h"
#include "input.h"
#include "sprites.h"
#include "fpg.h"

void init() {
    initDisplay();
    initCD();
    initMemory();
    initFont();
    initInput();

    initSprites();
    init_particles();
    
    init_fpgs();

}

void frame() {
    drawTexts();
    display();
}

