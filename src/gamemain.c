#include "core/hgl.h"
//#include "system.h"
#include "input/input.h"
#include "engine/sprites.h"
#include "engine/text.h"
#include "engine/tilemap.h"
#include "media/fpg.h"

#include "gameresources.h"

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
//#include "utils/async.h"
#include "utils/utils.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "levels/e1m1.h"
#include "levels/e1m1b.h"
#include "levels/e1m1c.h"

#define TILE_SIZE 16
#define HALF_TILE_SIZE (TILE_SIZE/2)
#define TILE(X) FIX32((X)*TILE_SIZE)
#define TILE_CENTER(X) FIX32((X)*TILE_SIZE + HALF_TILE_SIZE)
#define TILE_X_TO_SCREEN(X) (((X)*TILE_SIZE) - camposx)
#define TILE_CENTER_X_TO_SCREEN(X) (((X)*TILE_SIZE) + HALF_TILE_SIZE - camposx)
#define TILE_Y_TO_SCREEN(X) (((X)*TILE_SIZE) - camposy)
#define TILE_CENTER_Y_TO_SCREEN(X) (((X)*TILE_SIZE) + HALF_TILE_SIZE - camposy)
#define POS_TO_TILE_16(X) (fix32ToInt(X)>>4)

#define REPEAT5(X) X; X; X; X; X;
#define REPEAT10(X) REPEAT5(X) REPEAT5(X)
#define REPEAT25(X) REPEAT10(X) REPEAT10(X) REPEAT5(X)
#define REPEAT50(X) REPEAT25(X) REPEAT25(X)
#define REPEAT100(X) REPEAT50(X) REPEAT50(X)

#include "game.h"
#include "input/buttonstate.h"

Actor* spawnGoal(int tileX, int tileY); //main.c

void spawnBlockPrize(Tile tile) {
    if ((tile.tileX == 47 && tile.tileY == 24)) //Floor box
        spawnGoal(tile.tileX, tile.tileY - 1)->goal.mission = 1;
    else if (tile.tileX == 96 && tile.tileY == 8) //Cloud box
        spawnGoal(tile.tileX, tile.tileY - 1)->goal.mission = 2;
    else
        newBlock(Resources.getSmb3tTileset(), 1, TILE_CENTER(tile.tileX), TILE_CENTER(tile.tileY) - FIX32(16), BlockType_coin);
}

#define TILE_MASK_SOLID 0x80           //1000 0000
#define TILE_MASK_ONE_WAY 0x40         //0100 0000
#define TILE_MASK_BREAKABLE 0x20       //0010 0000
#define TILE_MASK_FLOOR 0xC0           //1100 0000
#define TILE_MASK_SOLID_BREAKABLE 0xA0 //1010 0000

void onPlayerCollidedWithCeilingTile(PlayerEventHandler*playerEventHandler, Tile tile) {
    uint8_t tileId;
    switch (tile.id) {
        case 0: break;
        case TILE_MASK_SOLID_BREAKABLE:
            tileId = *getTileAt(playerEventHandler->tilemap, tile.tileX, tile.tileY);

            switch (tileId) {
                case 42:
                case 77:
                    setTileAt(playerEventHandler->collisionTileMap, tile.tileX, tile.tileY, 0);
                    setTileAt(playerEventHandler->tilemap, tile.tileX, tile.tileY, 0);
                    int x = TILE_CENTER_X_TO_SCREEN(tile.tileX);
                    int y = TILE_CENTER_Y_TO_SCREEN(tile.tileY);
                    REPEAT25(new_Particle(x, y))
                    break;
                case 96:
                    break;
                default:
                    setTileAt(playerEventHandler->collisionTileMap, tile.tileX, tile.tileY, TILE_MASK_SOLID);
                    setTileAt(playerEventHandler->tilemap, tile.tileX, tile.tileY, 0);
                    //spawn block
                    newBlock(Resources.getSmb3tTileset(), 1, TILE_CENTER(tile.tileX), TILE_CENTER(tile.tileY), BlockType_block);
                    spawnBlockPrize(tile);

                    newSetUint8DelayedCommand(10, getTileAt(playerEventHandler->tilemap, tile.tileX, tile.tileY), 96);
                    break;
            }
            break;
        default:
            break;
    }
}

bool onPlayerCollidedWithFloorTile(PlayerEventHandler*playerEventHandler, Tile tile) {
    uint8_t tileId = *getTileAt(playerEventHandler->tilemap, tile.tileX, tile.tileY);
    switch (tileId) {
        case 0: break;
        case 42:
        case 77:
            playerEventHandler->player->sonic.doRebound();

            setTileAt(playerEventHandler->collisionTileMap, tile.tileX, tile.tileY, 0);
            setTileAt(playerEventHandler->tilemap, tile.tileX, tile.tileY, 0);
            int x = TILE_CENTER_X_TO_SCREEN(tile.tileX);
            int y = TILE_CENTER_Y_TO_SCREEN(tile.tileY);
            REPEAT25(new_Particle(x, y))
            return true;
            //break;
        default:
            break;
    }
    return false;
}

bool onPlayerGrounded(PlayerEventHandler*playerEventHandler, Tile tile) {
    return false;
}

void initPlayerEventHandler(PlayerEventHandler*playerEventHandler, TileMap *tilemap, TileMap* collisionTileMap) {
    playerEventHandler->collisionTileMap = collisionTileMap;
    playerEventHandler->tilemap = tilemap;
    playerEventHandler->onColidedWithCeilingTile = onPlayerCollidedWithCeilingTile;
    playerEventHandler->onColidedWithFloorTile = onPlayerCollidedWithFloorTile;
    playerEventHandler->onGrounded = onPlayerGrounded;
}

typedef enum {
    LevelIndex_PLAINS,
    LevelIndex_UNDERGROUND,
    LevelIndex_SECRET
} LevelIndex;

static int currentLevel = LevelIndex_PLAINS;
static int nextLevel = LevelIndex_PLAINS;


void checkCoin(TileMap* tileMap, Actor * actor) {
    if(currentLevel == LevelIndex_SECRET) return;

    int actorHalfWidth = FIX32(6);
    int actorHalfHeight = FIX32(6);
    int actorBoundingBoxY = FIX32(16);
    int minTileX = POS_TO_TILE_16(actor->entity->x - actorHalfWidth);
    int maxTileX = POS_TO_TILE_16(actor->entity->x + actorHalfWidth);
    int minTileY = POS_TO_TILE_16(actor->entity->y - actorBoundingBoxY);
    int maxTileY = POS_TO_TILE_16(actor->entity->y + actorHalfHeight);

    for (int tileY = minTileY; tileY <= maxTileY; tileY++) {
        for (int tileX = minTileX; tileX <= maxTileX; tileX++) {
            uint8_t* coinTile = getTileAt(tileMap, tileX, tileY);
            if (*coinTile == 7) {
                *coinTile = 0;
                REPEAT5(new_Particle(TILE_CENTER_X_TO_SCREEN(tileX), TILE_CENTER_Y_TO_SCREEN(tileY)))
            }
        }
    }
}

#include "engine/fsm.h"
CREATE_STATE_MACHINE(GameStateMachine, Menu, LoadLevel, Game, UnloadLevelBackToMenu, GameCompleted)

static int bgbx = 0;
static int bgby = 0;
static Actor * sonic = NULL;
static Actor * fallToBackgroundScript = NULL;
//static Actor * goal = NULL;
static TileMap bgaTileMap;
static TileMap collisionTileMap;
#define MAX_NUM_TILESETS_FPGS 9
static int tileset_fpgs[MAX_NUM_TILESETS_FPGS];
static AnimationState* tilesetAnimationState;

static HGL_Scroll * scroll;

//static int sonic_fpg, enemies_fpg;
static PlayerEventHandler playerEventHandler;

static void drawTestMenu() {
    int font_atlas = Resources.getFontAtlas();
    draw_text8(0, font_atlas, "hello this is a direct drawing", 32, 128, 0, -1);

    char * text = "hello this is a typewritter effect";

    static int typewritter = 0;
    static int typewritter1 = 0;
    static int typewritter2 = 0;
    static int typewritter3 = 0;

    if (typewritter < strlen(text)<<0) typewritter++; else typewritter = 0;
    if (typewritter1 < strlen(text)<<1) typewritter1++; else typewritter1 = 0;
    if (typewritter2 < strlen(text)<<2) typewritter2++; else typewritter2 = 0;
    if (typewritter3 < strlen(text)<<3) typewritter3++; else typewritter3 = 0;

    draw_text8(0, font_atlas, text, 16, 160, 0, typewritter);
    draw_text8(0, font_atlas, text, 16, 160+16, 0, typewritter1 >> 1);
    draw_text8(0, font_atlas, text, 16, 160+32, 0, typewritter2 >> 2);
    draw_text8(0, font_atlas, text, 16, 160+48, 0, typewritter3 >> 3);
}

GameState gameState = {
    .lastCourse = 0,
    .courseMissionState = {
        { active, hidden, hidden, hidden, hidden, hidden },
        { active, hidden, hidden, hidden, hidden, hidden },
        { active, hidden, hidden, hidden, hidden, hidden },
        { active, hidden, hidden, hidden, hidden, hidden },
        { active, hidden, hidden, hidden, hidden, hidden },
        { active, hidden, hidden, hidden, hidden, hidden },
        { active, hidden, hidden, hidden, hidden, hidden },
        { active, hidden, hidden, hidden, hidden, hidden },
        { active, hidden, hidden, hidden, hidden, hidden },
        { active, hidden, hidden, hidden, hidden, hidden }
    }
};

void initGameState() {
    gameState = (GameState) {
        .lastCourse = 0,
        .courseMissionState = {
            { active, hidden, hidden, hidden, hidden, hidden },
            { active, hidden, hidden, hidden, hidden, hidden },
            { active, hidden, hidden, hidden, hidden, hidden },
            { active, hidden, hidden, hidden, hidden, hidden },
            { active, hidden, hidden, hidden, hidden, hidden },
            { active, hidden, hidden, hidden, hidden, hidden },
            { active, hidden, hidden, hidden, hidden, hidden },
            { active, hidden, hidden, hidden, hidden, hidden },
            { active, hidden, hidden, hidden, hidden, hidden },
            { active, hidden, hidden, hidden, hidden, hidden }
        }
    };
}

CourseMenu courseMenu;

static HGL_Text * missionText;

static void loadCourseMenu(CourseMenu *courseMenu) {

    int oneStarsPositions[] = { 10 };
    int twoStarsPositions[] = { 9, 11 };
    int threeStarsPositions[] = { 8, 10, 12 };
    int fourStarsPositions[] = { 7, 9, 11, 13 };
    int fiveStarsPositions[] = { 6, 8, 10, 12, 14 };
    int sixStarsPositions[] = { 5, 7, 9, 11, 13, 15 };

    int* stars[6] = {
        oneStarsPositions,
        twoStarsPositions,
        threeStarsPositions,
        fourStarsPositions,
        fiveStarsPositions,
        sixStarsPositions
    };

    int numStars = courseMenu->numStars;
    const int *starPos = stars[numStars - 1];

    static const char* numbers[] = { "1", "2", "3", "4", "5", "6" };
    static const int numberY = 32;
    static const int missionY = 80;
    static const int courseY = 192;
    static const int numberXOffset = 4;

    int font_atlas = Resources.getFontAtlas();

    for (int i=0; i<numStars; i++) {
        HGL_TEXT_new(0, font_atlas, numbers[i], (TILE_SIZE * starPos[i]) - numberXOffset, numberY, 0, -1);
    }

    static const int screenCenterX = 320 >> 1;
    const char * missionDescription = courseMenu->missionDescription[courseMenu->selectedItem];
    int textPosX = text_get_centered_position(missionDescription, screenCenterX);
    int textPosX2 = text_get_centered_position(courseMenu->courseTitle, screenCenterX);

    missionText = HGL_TEXT_new(0, font_atlas, missionDescription, textPosX, missionY, 0, -1);
    HGL_TEXT_new(0, font_atlas, courseMenu->courseTitle, textPosX2, courseY, 0, -1);

    for (int i=0; i<numStars; i++) {
        Actor * star = newMenuStar(tileset_fpgs[0], TILE(starPos[i]), TILE(4));
        if (courseMenu->missionState[i] == completed) {
            star->menuStar.activate(star);
        } else {
            star->menuStar.deactivate(star);
        }
        courseMenu->stars[i] = star;
    }

    for (int i=0; i<numStars; i++){
        if(courseMenu->missionState[i] == active) {
            courseMenu->selectedItem = i;
            break;
        }
    }
    Actor * star = courseMenu->stars[courseMenu->selectedItem];
    star->menuStar.select(star);
}

bool isCourseCompleted(CourseMenu * courseMenu) {
    for (int i=0; i<courseMenu->numStars; i++){
        if(courseMenu->missionState[i] != completed) {
            return false;
        }
    }
    return true;
}

void updateMissionText(CourseMenu *courseMenu) {
    static const int screenCenterX = 320 >> 1;
    const char * missionDescription = courseMenu->missionDescription[courseMenu->selectedItem];
    missionText->x = text_get_centered_position(missionDescription, screenCenterX);
    missionText->text = missionDescription;
}

int updateCourseMenuInput(CourseMenu *courseMenu) {
    if (buttonState.just_pressed & PAD_START) return 1;
    if (courseMenu->numStars <= 1) return 0;

    if (buttonState.just_pressed & PAD_LEFT) {
        Actor * selectedStar = courseMenu->stars[courseMenu->selectedItem];
        selectedStar->menuStar.unselect(selectedStar);
        courseMenu->selectedItem--;
        while(courseMenu->missionState[courseMenu->selectedItem] == hidden && courseMenu->selectedItem >= 0) {
            courseMenu->selectedItem--;
        }
        if(courseMenu->selectedItem < 0) courseMenu->selectedItem = courseMenu->numStars - 1;
        selectedStar = courseMenu->stars[courseMenu->selectedItem];
        selectedStar->menuStar.select(selectedStar);

        updateMissionText(courseMenu);
    }
    if (buttonState.just_pressed & PAD_RIGHT) {
        Actor * selectedStar = courseMenu->stars[courseMenu->selectedItem];
        selectedStar->menuStar.unselect(selectedStar);
        courseMenu->selectedItem++;
        while(courseMenu->missionState[courseMenu->selectedItem] == hidden && courseMenu->selectedItem < courseMenu->numStars) {
            courseMenu->selectedItem++;
        }
        if(courseMenu->selectedItem >= courseMenu->numStars) courseMenu->selectedItem = 0;
        selectedStar = courseMenu->stars[courseMenu->selectedItem];
        selectedStar->menuStar.select(selectedStar);

        updateMissionText(courseMenu);
    }
    return 0;
}

static void drawCourseMenu(CourseMenu *courseMenu) {
    HGL_ANIM_updateAll();
    HGL_COMMAND_updateAll();
    HGL_ENT_updateAll();
    HGL_ACTOR_updateAll();
    HGL_ENT_renderAll(0, 0);
    HGL_SPR_renderAll();
    HGL_SCRIPT_updateAll();
    HGL_TEXT_renderAll();
    draw_all_sprites_basic();
}

Actor* spawnGoal(int tileX, int tileY) {
    return newGoalActivated(tileset_fpgs[0], TILE_CENTER(tileX), TILE_CENTER(tileY), sonic);
}

static void loadLevel_e1m1b() {
    bgaTileMap = fromTiledBin(smb3_1_1_underground_layer);
    collisionTileMap = fromTiledBin(smb3_1_1_underground_collision);

    bgaTileMap = cloneTileMap(&bgaTileMap);
    collisionTileMap = cloneTileMap(&collisionTileMap);

    setClearColor(0, 0, 0);

    bgbx = 0;
    bgby = 0;

    ANIM(TilesetAnimation, 4, 5, 6, 7)

    tilesetAnimationState = HGL_ANIM_new();
    SetAnimationState(tilesetAnimationState, TilesetAnimation, 8);

    scroll = HGL_SCROLL_new(tileset_fpgs[tilesetAnimationState->currentFrame], 1, &bgaTileMap, bgbx, bgby, 6, 0);

    initPlayerEventHandler(&playerEventHandler, &bgaTileMap, &collisionTileMap);


    sonic = newSonic(Resources.getSonicFpg(), TILE(9), TILE(3), collisionTileMap, &playerEventHandler);
    playerEventHandler.player = sonic;
    sonicDoEnterFromPipe();

    MAP_WIDTH  = bgaTileMap.numCols * TILE_SIZE;
    MAP_HEIGHT = bgaTileMap.numRows * TILE_SIZE;
    newCamera(sonic, FIX32(1), FIX32(0));
    //camposx = TILE_SIZE * 6;
    //camposy = 0;

    newGoalFloating(tileset_fpgs[0], TILE(23), TILE(5), sonic)->goal.mission = 3;

    nextLevel = LevelIndex_PLAINS;
}

static void openChestTriggerCallback(Actor* trigger) {
    deleteActor(trigger);
    setTileAt(&bgaTileMap, 7, 9, 0x09);
    setTileAt(&bgaTileMap, 8, 9, 0x0a);
}

static void loadLevel_e1m1c() {
    bgaTileMap = fromTiledBin(e1m1c_layer);
    collisionTileMap = fromTiledBin(e1m1c_collision);

    bgaTileMap = cloneTileMap(&bgaTileMap);
    collisionTileMap = cloneTileMap(&collisionTileMap);

    setClearColor(0, 0, 0);

    bgbx = 0;
    bgby = 0;

    ANIM(TilesetAnimation, 8)
    tilesetAnimationState = HGL_ANIM_new();
    SetAnimationState(tilesetAnimationState, TilesetAnimation, 8);

    scroll = HGL_SCROLL_new(tileset_fpgs[tilesetAnimationState->currentFrame], 1, &bgaTileMap, bgbx, bgby, 6, 0);

    initPlayerEventHandler(&playerEventHandler, &bgaTileMap, &collisionTileMap);

    sonic = newSonic(Resources.getSonicFpg(), TILE(2), TILE(6), collisionTileMap, &playerEventHandler);
    playerEventHandler.player = sonic;

    MAP_WIDTH  = bgaTileMap.numCols * TILE_SIZE;
    MAP_HEIGHT = bgaTileMap.numRows * TILE_SIZE;
    //newCamera(sonic, FIX32(6), FIX32(0));
    camposx = TILE_SIZE * -2;
    camposy = TILE_SIZE * -2;

    newTriggerScript(&playerEventHandler, openChestTriggerCallback, true, (AABB) { TILE_SIZE * 7, TILE_SIZE * 9, TILE_SIZE * 2, TILE_SIZE * 2});
    newGoalHiddenChest(tileset_fpgs[0], TILE(8), TILE(9), sonic)->goal.mission = 5;

    nextLevel = LevelIndex_PLAINS;
}

static void loadUndergroundLevelTriggerCallback(Actor * trigger);
static void loadSecretLevelTriggerCallback(Actor * trigger);

static void loadLevel_e1m1() {
    bgaTileMap = fromTiledBin(smb3_2_layer);
    collisionTileMap = fromTiledBin(smb3_2_collision);

    bgaTileMap = cloneTileMap(&bgaTileMap);
    collisionTileMap = cloneTileMap(&collisionTileMap);

    setClearColor(175, 249, 240);

    bgbx = 0;
    bgby = 0;

    ANIM(TilesetAnimation, 0, 1, 2, 3)

    tilesetAnimationState = HGL_ANIM_new();
    SetAnimationState(tilesetAnimationState, TilesetAnimation, 8);

    scroll = HGL_SCROLL_new(tileset_fpgs[tilesetAnimationState->currentFrame], 1, &bgaTileMap, bgbx, bgby, 6, 0);

    initPlayerEventHandler(&playerEventHandler, &bgaTileMap, &collisionTileMap);

    newTriggerScript(&playerEventHandler, loadUndergroundLevelTriggerCallback, true, (AABB) { TILE_SIZE * 147 + 8, TILE_SIZE * 6, TILE_SIZE * 1,  TILE_SIZE * 1});

    //sonic = newSonic(Resources.getSonicFpg(), TILE(6), TILE(25), collisionTileMap, &playerEventHandler);
    sonic = newSonic(Resources.getSonicFpg(), TILE(148), TILE(6), collisionTileMap, &playerEventHandler);
    playerEventHandler.player = sonic;

    int enemies_fpg = Resources.getEnemiesFpg();
    newMotobug(enemies_fpg, TILE(20),TILE(25));
    newMotobug(enemies_fpg, TILE(64),TILE(24));
    newMotobug (enemies_fpg, TILE(90), TILE(25));
    newBee(enemies_fpg, TILE(130),TILE(19));

    newGoal(tileset_fpgs[0], TILE_CENTER(174), TILE_CENTER(21), sonic)->goal.mission = 0; // 1 - Finish

    //newGoal(tileset_fpgs[0], TILE_CENTER(47), TILE_CENTER(24), sonic)->goal.mission = 1; // 2 -  Floor box
    //setTileAt(&bgaTileMap, 47, 24, 0); //Hide floor box
    //setTileAt(&collisionTileMap, 47, 24, 0); //Hide floor box

    //newGoal(tileset_fpgs[0], TILE_CENTER(96), TILE_CENTER(8), sonic)->goal.mission = 2; // 3 -  cloud box
    //newGoalFloating(tileset_fpgs[0], TILE(148), TILE_CENTER(4), sonic)->goal.mission = 3; // 4 -  pipe

    Actor * hiddenGoal = newGoalHiddenInactive(tileset_fpgs[0], TILE_CENTER(65), TILE_CENTER(24), sonic); // 5 - behind bushes
    hiddenGoal->goal.mission = 4; // 5 - behind bushes

    //Actor * hiddenGoal2 = newGoalHiddenInactive(tileset_fpgs[0], TILE_CENTER(177), TILE_CENTER(25), sonic);
    //hiddenGoal2->goal.mission = 5; // 6 - Gotta go fast
    Actor * trigger = newTriggerScript(&playerEventHandler, loadSecretLevelTriggerCallback, false, (AABB) { TILE_SIZE * 174, 0, 160, 416});
    fallToBackgroundScript = newFallToBackgroundScript(
        &playerEventHandler,hiddenGoal, trigger, bgaTileMap,
        (AABB) { TILE_SIZE * 37, TILE_SIZE * 18, TILE_SIZE * 6, TILE_SIZE *1});


    //spawnGoal(174, 21);

    MAP_WIDTH  = bgaTileMap.numCols * TILE_SIZE;
    MAP_HEIGHT = bgaTileMap.numRows * TILE_SIZE;
    newCamera(sonic, FIX32(40), FIX32(128));

    //Actor * tileShader = newTileShader(&bgaTileMap);
}

typedef void (*functionPointer)();
static functionPointer levels[] = {
    loadLevel_e1m1,
    loadLevel_e1m1b,
    loadLevel_e1m1c
};

static void unloadLevel() {
    HGL_ACTOR_deleteAll();
    HGL_COMMAND_deleteAll();
    HGL_TEXT_deleteAll();
    HGL_SCROLL_deleteAll();
    HGL_SCRIPT_deleteAll();
    HGL_ANIM_deleteAll();
    remove_Particles();
    freeTileMap(&bgaTileMap);
    freeTileMap(&collisionTileMap);
    initSprites();
}

#include "utils/async.h"
#include "utils/script.h"

/*
#define FRAME yield(0)
#define WAIT_FRAMES(X) \
    int _coroutine_frame_delay = (X); \
    while(_coroutine_frame_delay-- > 0) {  yield(0); }\

#define repeat do
#define until(exp) while(!(exp))

#define waitUntil(X) repeat { FRAME; } until (X);
#define waitForInput(X) waitUntil(X);
*/

//#define await_frames(X) \
    //int _coroutine_frame_delay = (X); \ // you can't declare variables here
    //await_while(_coroutine_frame_delay-- > 0)
#define script_awaitFadeIn() \
    fadeIn(); \
    script_await(isFading());

#define script_awaitFade(fadeFun) \
    fadeFun(); \
    script_await(fadeFinished()); 

#define async_awaitFadeIn() \
    fadeIn(); \
    await(isFading());

#define async_awaitFade(fadeFun) \
    fadeFun(); \
    await(fadeFinished()); 

ASYNC_SCRIPT(goToMainMenuScript)
    int8_t mission = (int8_t)asyncScript->data;

    printf("Completed mission %i\n", mission);

    CourseMissionState * courseMissionState = &gameState.courseMissionState[0];

    CourseMissionState_completeMission(courseMissionState, mission);
    CourseMissionState_activateMission(courseMissionState, CourseMissionState_getNextMission(courseMissionState));

    async_awaitFade(whiteFadeOut);
    GameStateMachine.setUnloadLevelBackToMenu();
ASYNC_SCRIPT_END

void doPlayerWinAnimationParticles(Actor*player, int8_t mission) {
    int x = TILE_CENTER_X_TO_SCREEN(POS_TO_TILE_16(player->entity->x));
    int y = TILE_CENTER_Y_TO_SCREEN(POS_TO_TILE_16(player->entity->y));
    REPEAT25(new_Particle(x, y))
    HGL_SCRIPT_create(120, goToMainMenuScript, NULL, mission);
}

ASYNC_SCRIPT(loadLevelScript)
    async_awaitFade(blackFadeOut);
    nextLevel = asyncScript->data;
    GameStateMachine.setLoadLevel();
ASYNC_SCRIPT_END

static void loadUndergroundLevelTriggerCallback(Actor * trigger) {
    if (buttonState.btn & PAD_DOWN) {
        deleteActor(trigger);
        sonic->sonic.doPipeDown(FIX32(16 * 148));
        HGL_SCRIPT_create(60, loadLevelScript, NULL, LevelIndex_UNDERGROUND);
    }
}

static void loadSecretLevelTriggerCallback(Actor* trigger) {
    deleteActor(trigger);
    HGL_SCRIPT_create(0, loadLevelScript, NULL, LevelIndex_SECRET);
}

#define fsm_script_begin script_begin_with(&GameStateMachine.asyncState);

static void stateMenu() {
    updateCourseMenuInput(&courseMenu);
    fsm_script_begin
        setClearColor(175, 249, 240);
        initCourseMenu(&courseMenu, &gameData.course[0], &gameState);
        loadCourseMenu(&courseMenu);
        script_awaitFade(fadeIn);
        script_await(buttonState.just_pressed & PAD_START);
        script_awaitFade(whiteFadeOut)
        if(isCourseCompleted(&courseMenu)) GameStateMachine.setGameCompleted(); else GameStateMachine.setLoadLevel();
        HGL_ACTOR_deleteAll();
        HGL_TEXT_deleteAll();
        HGL_ANIM_deleteAll();
        initSprites();
    script_end;

    drawCourseMenu(&courseMenu);
}

static void stateLoadLevel() {
    unloadLevel();
    currentLevel = nextLevel;
    levels[nextLevel]();
    GameStateMachine.setGame();
}

static void stateUnloadLevelBackToMenu() {
    unloadLevel();
    GameStateMachine.setMenu();
}

static void stateGameCompleted() { }

void draw_all_sprites_zorder2();

static void stateGameUpdate() {
    sonic->sonic.inputHandler.handleInput(&buttonState);
    if(fallToBackgroundScript) fallToBackgroundScript->inputHandler.handleInput(&buttonState);

    HGL_COMMAND_updateAll();
    HGL_SCRIPT_updateAll();
    HGL_ANIM_updateAll();

    bgbx = camposx;
    bgby = camposy;

    scroll->file = tileset_fpgs[tilesetAnimationState->currentFrame];
    HGL_SCROLL_setOffset(scroll, bgbx, bgby);

    HGL_ENT_updateAll();
    HGL_ACTOR_updateAll();

    //Handle collisions
    checkCoin(&bgaTileMap, sonic);
    //checkGoal( goal, sonic);

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

    update_Particles();

    int font_atlas = Resources.getFontAtlas();
    draw_text8(0, font_atlas, "012345789", 8, 8, 0, -1);
    draw_text8(0, font_atlas, "ABCDEFGHIJLMNOPQRSTUVWXYZ", 8, 16, 0, -1);
    draw_text8(0, font_atlas, "Hello", 8, 24, 0, -1);
}

static void stateGame() {
    stateGameUpdate();
    fsm_script_begin
        script_awaitFadeIn();
        script_await(buttonState.just_pressed & PAD_START);
        script_awaitFade(whiteFadeOut);
        GameStateMachine.setUnloadLevelBackToMenu();
    script_end;
}

int gameMain() {
    printf("gameMain\n");

    tileset_fpgs[0] = Resources.getSmb3tTileset();
    tileset_fpgs[1] = Resources.getSmb3t2Tileset();
    tileset_fpgs[2] = Resources.getSmb3t3Tileset();
    tileset_fpgs[3] = Resources.getSmb3t4Tileset();

    tileset_fpgs[4] = Resources.getSmb3uTileset();
    tileset_fpgs[5] = tileset_fpgs[4];
    tileset_fpgs[6] = tileset_fpgs[4];
    tileset_fpgs[7] = tileset_fpgs[4];

    tileset_fpgs[8] = Resources.getSmb3hTileset();

    initGameStateMachine();

    return 0;
}

int gameUpdate() {
    if (isFaded()) {
        initButtonStateInput();
    } else {
        updateInput();
    }
    GameStateMachine.update();
    updateFader();
    HGL_frame();
    return isNotGameCompleted();
}
