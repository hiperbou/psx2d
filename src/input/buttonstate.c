#include "input.h"
#include "buttonstate.h"

void updateButtonState(ButtonState *buttonState) {
    int currentState = getButtons(0);
    int last = buttonState->last;
    *buttonState = (ButtonState) {
            .btn = currentState,
            .changed = currentState ^ last,
            .just_pressed = currentState & ~last,
            .released = ~currentState & last,
            .last = currentState
    };
}