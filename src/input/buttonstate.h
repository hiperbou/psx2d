#ifndef __INPUT_BUTTON_STATE_H__
#define __INPUT_BUTTON_STATE_H__

#include <stdint.h>

typedef struct {
    uint16_t btn;
    uint16_t changed;
    uint16_t just_pressed;
    uint16_t released;
    uint16_t last;
} ButtonState;

void initButtonState(ButtonState *buttonState);
void updateButtonState(ButtonState *buttonState);

#endif //__INPUT_BUTTON_STATE_H__