#include <kos.h>
#include <dc/maple.h>
#include <dc/maple/controller.h>
#include "../../input/input.h"


void initInput() {
    // KOS handles this automatically
}

uint16_t getButtons(int player) {
    maple_device_t *cont;
    cont_state_t *state;

    cont = maple_enum_type(player, MAPLE_FUNC_CONTROLLER);

    if (cont) {
        state = (cont_state_t *)maple_dev_status(cont);
        if (state) {
            return state->buttons;
        }
    }
    return 0;
}
