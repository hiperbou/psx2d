#include "core/hgl.h"

#include "input/buttonstate.h"

#include "input/input.h"
#include "engine/sprites.h"
#include "engine/text.h"
#include "engine/tilemap.h"
#include "media/fpg.h"

#include "game/sonic.h"
#include "game/fallToBackgroundScript.h"
#include "game/triggerScript.h"

#include "core/hgl_types.h"

#include "core/hgl_anim.h"
#include "core/hgl_command.h"
#include "core/hgl_mem.h"
#include "core/hgl_scroll.h"
#include "core/hgl_text.h"
#include "engine/fader.h"

#include "game/actors.h"
#include "game/camera.h"
#include "game/tileshader.h"
#include "game/data/gamedata.h"
#include "game/state/gamestate.h"
#include "game/menu/coursemenu.h"
#include "utils/utils.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

ButtonState buttonState;

void initButtonStateInput() {
    initButtonState(&buttonState);
}

void updateInput() {
    updateButtonState(&buttonState);
}

void gameInit() {
    printf("gameInit\n");

    initButtonStateInput();

    HGL_init();

    HGL_COMMAND_init();
    HGL_ANIM_init();
    HGL_SPR_init();
    HGL_ENT_init();
    HGL_ACTOR_init();
    HGL_SCROLL_init();
    HGL_TEXT_init();
}