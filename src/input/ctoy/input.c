
#include "../input.h"
#include <ctoy.h>

void initInput() {

}

PADSTATE * getPadState(int player) {
    return 0;//(PADSTATE*)padbuff[player];
}

uint16_t getButtons(int player) {
    return (ctoy_key_pressed(CTOY_KEY_LEFT) ? PAD_LEFT : 0) |
            (ctoy_key_pressed(CTOY_KEY_RIGHT) ? PAD_RIGHT : 0) |
            (ctoy_key_pressed(CTOY_KEY_UP) ? PAD_UP: 0) |
            (ctoy_key_pressed(CTOY_KEY_DOWN) ? PAD_DOWN: 0) |
            (ctoy_key_pressed(CTOY_KEY_X) ? PAD_CROSS: 0) |
            (ctoy_key_pressed(CTOY_KEY_Z) ? PAD_SQUARE: 0) |
            (ctoy_key_pressed(CTOY_KEY_ENTER) ? PAD_START: 0);
}
