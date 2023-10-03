#include "input.h"
#include "buttonstate.h"
#include <memory.h>

void initButtonState(ButtonState *buttonState) {
    memset(buttonState, 0, sizeof (ButtonState));
}

void updateButtonState(ButtonState *buttonState) {
    int currentState = getButtons(0);
    int last = buttonState->btn;
    *buttonState = (ButtonState) {
            .btn = currentState,
            .changed = currentState ^ last,
            .just_pressed = currentState & ~last,
            .released = ~currentState & last,
            .last = last
    };
}