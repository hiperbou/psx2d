#include "../input.h"
#include "raylib.h"

void initInput() {}

PADSTATE * getPadState(int player) {
    return 0;//(PADSTATE*)padbuff[player];
}

uint16_t getButtons(int player) {
    return (IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT) ? PAD_LEFT : 0) |
            (IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT) ? PAD_RIGHT : 0) |
            (IsKeyDown(KEY_UP) ? PAD_UP: 0) |
            (IsKeyDown(KEY_DOWN) ? PAD_DOWN: 0) |
            (IsKeyDown(KEY_X) ? PAD_CROSS: 0) |
            (IsKeyDown(KEY_Z) ? PAD_SQUARE: 0) |
            (IsKeyDown(KEY_ENTER) ? PAD_START: 0);
}
