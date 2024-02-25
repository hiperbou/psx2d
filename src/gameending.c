#include "game.h"
#include "gameresources.h"

#include "core/hgl.h"

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

#include "utils/picoro.h"

#include "engine/fsm.h"
CREATE_STATE_MACHINE(EndingStateMachine, Ending, UnloadEnding, ExitEnding)



static void stateEnding() {
    if (buttonState.just_pressed & PAD_START) {
        whiteFadeOut();
        setUnloadEnding();
    }

    int font_atlas =  Resources.getFontAtlas();
    draw_text8(0, font_atlas, "THANKS", 8, 108, 0, -1);
    draw_text8(0, font_atlas, "FOR", 8, 116, 0, -1);
    draw_text8(0, font_atlas, "PLAYING!", 8, 124, 0, -1);
}

static void stateUnloadEnding() {
    if(isNotFading()) setExitEnding();
}

static void stateExitEnding() { }

int gameEndingUpdate() {
    if (isFaded()) {
        initButtonStateInput();
    } else {
        updateInput();
    }
    EndingStateMachine.update();
    updateFader();
    HGL_frame();
    return isNotExitEnding();
}

int gameEnding() {
    printf("gameEnding\n");

    initEndingStateMachine();

    fadeIn();
    return 0;
}