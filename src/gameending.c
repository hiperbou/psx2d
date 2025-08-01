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
#include "core/hgl_script.h"
#include "core/hgl_text.h"
#include "engine/fader.h"

#include "game/actors.h"
#include "game/camera.h"
#include "game/tileshader.h"
#include "game/data/gamedata.h"
#include "game/state/gamestate.h"
#include "game/menu/coursemenu.h"
#include "utils/utils.h"
#include "utils/async.h"
#include "utils/script.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

static AsyncState asyncState = { ASYNC_INIT };

async endingAsync(AsyncState* asyncState) { 
    int font_atlas =  Resources.getFontAtlas();
    draw_text8(0, font_atlas, "THANK YOU SO MUCH A-FOR-TO", 8, 108, 0, -1);
    draw_text8(0, font_atlas, "PLAYING MY GAME!", 8, 116, 0, -1);
    draw_text8(0, font_atlas, "HIPERBOU", 8, 132, 0, -1);

    async_begin(asyncState);
        async_awaitFadeIn();
        async_await(buttonState.just_pressed & PAD_START);
        async_awaitFade(whiteFadeOut);
    async_end;
}

int gameEndingUpdate() {
    if (isFaded()) {
        initButtonStateInput();
    } else {
        updateInput();
    }

    endingAsync(&asyncState);

    updateFader();
    HGL_frame();
    return async_not_done(&asyncState);
}

int gameEnding() {
    printf("gameEnding\n");
    async_init(&asyncState);
    return 0;
}