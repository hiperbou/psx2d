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

#include "levels/e1m1c.h"
#include "levels/intro.h"
#include "levels/introground.h"

#include "utils/picoro.h"

#include "engine/fsm.h"
CREATE_STATE_MACHINE(TitleStateMachine, Title, UnloadTitle, LoadGame)

CREATE_STATE_MACHINE(SequenceStateMachine, SeqInit, SeqStart, SeqUp, SeqWait, SeqTitleFall, SeqShake, SeqStartFade, SeqFade, SeqFinishFade, SeqPressStart)

static TileMap bgaTileMap;
static TileMap bgbTileMap;
static int bgbx = 0;
static int bgby = 0;
static HGL_Scroll * scroll;
static HGL_Scroll * groundScroll;

int getBackgroundFramePosX(int frame) {
    #define MARGIN -32
    return MARGIN + 256 * frame;
}

static int seqDelay = 0;
static void stateSeqInit() {
    bgbx = getBackgroundFramePosX(0);
    bgby = 0;
    seqDelay = 30;
    setSeqStart();
}

static void stateSeqStart() {
    if(--seqDelay == 0) setSeqUp();
}

static void stateSeqUp() {
    bgby+=2;
    if(bgby == 240)  { 
        seqDelay = 10;
        setSeqWait();
    }
}

static void stateSeqWait() {
    if(--seqDelay == 0) setSeqTitleFall();
}

static void stateSeqTitleFall() {
    bgbx = getBackgroundFramePosX(1);
    bgby-=6;
    if(bgby <= 0)  { 
        bgby = 0;
        seqDelay = 30;
        setSeqShake();
    }
}

static void stateSeqShake() {
    bgbx = getBackgroundFramePosX(1);
    bgby = seqDelay % 4;
    if(--seqDelay == 0)  { 
        seqDelay = 30;
        setSeqStartFade();
    }
}

static void stateSeqStartFade() {
    bgbx = getBackgroundFramePosX(1);
    if(--seqDelay == 0) {
        seqDelay = 16;
        setSeqFade();
    }
}

static void stateSeqFade() {
    static uint8_t targetColor [] = { 255,219,161 };
    int d = seqDelay>>2;
    setClearColor(targetColor[0] >> d, targetColor[1]  >> d, targetColor[2] >> d);

    if(seqDelay==7) bgbx = getBackgroundFramePosX(2);
    if(--seqDelay == 0) {
        seqDelay = 30;
        setSeqFinishFade();
    }
}

static void stateSeqFinishFade() {
    if(--seqDelay == 0) setSeqPressStart();
}

static void stateSeqPressStart() {
    bgbx = getBackgroundFramePosX(3 + (seqDelay>>4) % 2);
    seqDelay++;
}


static void loadLevel_intro() {
    bgaTileMap = fromTiledBin(intro_layer);
    bgbTileMap = fromTiledBin(introground_layer);
    //collisionTileMap = fromTiledBin(smb3_1_1_underground_collision);

    bgaTileMap = cloneTileMap(&bgaTileMap);
    //collisionTileMap = cloneTileMap(&collisionTileMap);

    setClearColor(0, 0, 0);

    bgbx = 0;
    bgby = 0;

    groundScroll = HGL_SCROLL_new(Resources.getIntroTileset(), 1, &bgbTileMap, 0, 0, 6, 0);
    scroll = HGL_SCROLL_new(Resources.getIntroTileset(), 1, &bgaTileMap, bgbx, bgby, 6, 0);

}

static void stateTitle() {
    if (buttonState.just_pressed & PAD_START) {
        whiteFadeOut();
        setUnloadTitle();
    }

    SequenceStateMachine.update();

    HGL_SCROLL_setOffset(scroll, bgbx, bgby);

    HGL_ENT_updateAll();
    HGL_ACTOR_updateAll();

    HGL_ENT_renderAll(bgbx,bgby);
    HGL_SCROLL_renderAll();
    HGL_SPR_renderAll();
    HGL_TEXT_renderAll();

#ifdef PSX
    //draw_tilemap_no_wrap(tileset_fpgs[tilesetAnimationState->currentFrame], 1, &bgaTileMap, bgbx, bgby, 0); //Front
    //draw_tilemap_no_wrap(tileset_fpgs[4 + tilesetAnimationState->currentFrame], 1, &bgaTileMap, bgbx, bgby, 0); //Front
    draw_all_sprites_basic();
#else //CToy
    //draw_tilemap_no_wrap(tileset_fpgs[tilesetAnimationState->currentFrame], 1, &bgaTileMap, bgbx, bgby, 0); //Front
    //draw_all_sprites_zorder();
    //draw_all_sprites_zorder2();
    draw_all_sprites_layer();
#endif
}

static void stateUnloadTitle() {
    if(isFading()) return;

    HGL_ACTOR_deleteAll();
    HGL_COMMAND_deleteAll();
    HGL_TEXT_deleteAll();
    HGL_SCROLL_deleteAll();
    HGL_ANIM_deleteAll();
    remove_Particles();
    HGL_free((void*)bgaTileMap.map);
    //HGL_free((void*)collisionTileMap.map);
    initSprites();

    setLoadGame();
}

static void stateLoadGame() { }

int gameTitleUpdate() {
    if (isFaded()) {
        initButtonStateInput();
    } else {
        updateInput();
    }
#ifdef __EMSCRIPTEN__
    startFrame(); // Added for Emscripten
#endif
    TitleStateMachine.update();
    updateFader();
    HGL_frame();
    return isNotLoadGame();
}

int gameTitle() {
    printf("gameTitle\n");
    loadLevel_intro();

    initTitleStateMachine();
    initSequenceStateMachine();
    return 0;
}