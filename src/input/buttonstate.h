#ifndef __INPUT_BUTTON_STATE_H__
#define __INPUT_BUTTON_STATE_H__

#include <stdint.h>

typedef struct {
    uint8_t btn;
    uint8_t changed;
    uint8_t just_pressed;
    uint8_t released;
    uint8_t last;
} ButtonState;

void updateButtonState(ButtonState *buttonState);

#endif //__INPUT_BUTTON_STATE_H__