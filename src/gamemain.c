#include "core/hgl.h"
//#include "system.h"
#include "input/input.h"
#include "engine/sprites.h"
#include "engine/text.h"
#include "engine/tilemap.h"
#include "media/fpg.h"

#include "game/sonic.h"
#include "game/fallToBackgroundScript.h"

#include "core/hgl_types.h"

#include "game/actors.h"
#include "core/hgl_anim.h"
#include "core/hgl_command.h"
#include "core/hgl_mem.h"
#include "game/camera.h"
#include "game/tileshader.h"
#include "game/data/gamedata.h"
#include "game/state/gamestate.h"
#include "game/menu/coursemenu.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "smb3scene.h"
#include "smb3col.h"


//#include "memory.h"

void wait(char *message) {
    int wait = 20;
    while(wait) {
        //FntPrint(message);
        wait--;
        HGL_frame();
    }
}

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
#define REPEAT10(X) REPEAT5(X); REPEAT5(X);
#define REPEAT25(X) REPEAT10(X); REPEAT10(X); REPEAT5(X);
#define REPEAT50(X) REPEAT25(X); REPEAT25(X);
#define REPEAT100(X) REPEAT50(X); REPEAT50(X);

#include "input/buttonstate.h"

static ButtonState buttonState;

static void initButtonStateInput() {
    initButtonState(&buttonState);
}

static void updateInput() {
    updateButtonState(&buttonState);
}

Actor* spawnGoal(int tileX, int tileY); //main.c

void spawnBlockPrize(Tile tile) {
    if ((tile.tileX == 47 && tile.tileY == 24)) //Floor box
        spawnGoal(tile.tileX, tile.tileY - 1)->goal.mission = 1;
    else if (tile.tileX == 96 && tile.tileY == 8) //Cloud box
        spawnGoal(tile.tileX, tile.tileY - 1)->goal.mission = 2;
    else
        newBlock(2, 1, TILE_CENTER(tile.tileX), TILE_CENTER(tile.tileY) - FIX32(16), BlockType_coin);
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
                    newBlock(2, 1, TILE_CENTER(tile.tileX), TILE_CENTER(tile.tileY), BlockType_block);
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

void goToMainMenu();

void newGoToMainMenuCommand(int delay, int8_t value) {
    DelayedCommand * command = HGL_COMMAND_new();
    *command = (DelayedCommand) {
            .delay = delay,
            .callback = goToMainMenu,
            .target = NULL,
            .data = value
    };
}

void doPlayerWinAnimationParticles(Actor*player, int8_t mission) {
    int x = TILE_CENTER_X_TO_SCREEN(POS_TO_TILE_16(player->entity->x));
    int y = TILE_CENTER_Y_TO_SCREEN(POS_TO_TILE_16(player->entity->y));
    REPEAT25(new_Particle(x, y))
    newGoToMainMenuCommand(120, mission);
}

void initPlayerEventHandler(PlayerEventHandler*playerEventHandler, TileMap *tilemap, TileMap* collisionTileMap) {
    playerEventHandler->collisionTileMap = collisionTileMap;
    playerEventHandler->tilemap = tilemap;
    playerEventHandler->onColidedWithCeilingTile = onPlayerCollidedWithCeilingTile;
    playerEventHandler->onColidedWithFloorTile = onPlayerCollidedWithFloorTile;
    playerEventHandler->onGrounded = onPlayerGrounded;
}


void checkCoin(TileMap* tileMap, Actor * actor) {
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
                *coinTile = 1;
                REPEAT5(new_Particle(TILE_CENTER_X_TO_SCREEN(tileX), TILE_CENTER_Y_TO_SCREEN(tileY)))
            }
        }
    }
}

#include "engine/fsm.h"
CREATE_STATE_MACHINE(GameStateMachine, LoadMenu, Menu, LoadLevel, Game, UnloadLevel)


static int font_atlas;



static int bgbx = 0;
static int bgby = 0;
static Actor * sonic = NULL;
static Actor * fallToBackgroundScript = NULL;
//static Actor * goal = NULL;
static TileMap bgaTileMap;
static TileMap collisionTileMap;
static int tileset_fpgs[4];
static AnimationState* tilesetAnimationState;

static int sonic_fpg, enemies_fpg;
static PlayerEventHandler playerEventHandler;

static void drawTestMenu() {
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

CourseMenu courseMenu;

static void loadCourseMenu(CourseMenu *courseMenu) {

    int oneStarsPositions[] = { 10 };
    int twoStarsPositions[] = { 9, 11 };
    int threeStarsPositions[] = {8, 10, 12};
    int fourStarsPositions[] = {7, 9, 11, 13};
    int fiveStarsPositions[] = { 6, 8, 10, 12, 14};
    int sixStarsPositions[] = {5, 7, 9, 11, 13, 15};

    int* stars[6] = {
        oneStarsPositions,
        twoStarsPositions,
        threeStarsPositions,
        fourStarsPositions,
        fiveStarsPositions,
        sixStarsPositions
    };

    int numStars = courseMenu->numStars;
    int *starPos = stars[numStars - 1];

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

void updateCourseMenuInput(CourseMenu *courseMenu) {
    if (courseMenu->numStars <= 1) return;

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
    }
}

static void drawCourseMenu(CourseMenu *courseMenu) {
    static const int numberY = 32;
    static const int missionY = 80;
    static const int courseY = 192;

    static const int oneStarsPositions[] = { 10 };
    static const int twoStarsPositions[] = { 9, 11 };
    static const int threeStarsPositions[] = {8, 10, 12};
    static const int fourStarsPositions[] = {7, 9, 11, 13};
    static const int fiveStarsPositions[] = { 6, 8, 10, 12, 14};
    static const int sixStarsPositions[] = {5, 7, 9, 11, 13, 15};

    static const int* stars[6] = {
            oneStarsPositions,
            twoStarsPositions,
            threeStarsPositions,
            fourStarsPositions,
            fiveStarsPositions,
            sixStarsPositions
    };
    static const char* numbers[] = { "1", "2", "3", "4", "5", "6" };
    static const int numberXOffset = 4;

    int numStars = courseMenu->numStars;
    const int *starPos = stars[numStars - 1];
    for (int i=0; i<numStars; i++) {
        draw_text8(0, font_atlas, numbers[i], (TILE_SIZE * starPos[i]) - numberXOffset, numberY, 0, -1);
    }

    //draw_text8(0, font_atlas, "1", 0, numberY, 0, -1);
    static const int screenCenterX = 320 >> 1;
    const char * missionText = courseMenu->missionDescription[courseMenu->selectedItem];
    int textPosX = text_get_centered_position(missionText, screenCenterX);
    int textPosX2 = text_get_centered_position(courseMenu->courseTitle, screenCenterX);

    draw_text8(0, font_atlas, missionText, textPosX, missionY, 0, -1);
    draw_text8(0, font_atlas, courseMenu->courseTitle, textPosX2, courseY, 0, -1);


    HGL_ANIM_updateAll();
    HGL_ENT_updateAll();
    HGL_ACTOR_updateAll();
    HGL_ENT_renderAll(0, 0);
    HGL_SPR_renderAll();
    draw_all_sprites_basic();
}


Actor* spawnGoal(int tileX, int tileY) {
    return newGoalActivated(tileset_fpgs[0], TILE_CENTER(tileX), TILE_CENTER(tileY), sonic);
}

static void loadLevel() {
    bgaTileMap = fromTiledBinScene(smb3scene,28);
    collisionTileMap = fromTiledBinScene(smb3col,32);

    bgaTileMap = cloneTileMap(&bgaTileMap);
    collisionTileMap = cloneTileMap(&collisionTileMap);

    bgbx = 0;
    bgby = 0;

    initPlayerEventHandler(&playerEventHandler, &bgaTileMap, &collisionTileMap);

    sonic = newSonic(sonic_fpg, TILE(6), TILE(25), collisionTileMap, &playerEventHandler);
    playerEventHandler.player = sonic;


    newMotobug(enemies_fpg, TILE(20),TILE(25));
    newMotobug(enemies_fpg, TILE(64),TILE(24));
    newMotobug (enemies_fpg, TILE(90), TILE(25));
    newBee(enemies_fpg, TILE(130),TILE(19));

    newGoal(tileset_fpgs[0], TILE_CENTER(174), TILE_CENTER(21), sonic)->goal.mission = 0; // 1 - Finish

    //newGoal(tileset_fpgs[0], TILE_CENTER(47), TILE_CENTER(24), sonic)->goal.mission = 1; // 2 -  Floor box
    //setTileAt(&bgaTileMap, 47, 24, 0); //Hide floor box
    //setTileAt(&collisionTileMap, 47, 24, 0); //Hide floor box

    //newGoal(tileset_fpgs[0], TILE_CENTER(96), TILE_CENTER(8), sonic)->goal.mission = 2; // 3 -  cloud box
    newGoalActivated(tileset_fpgs[0], TILE(148), TILE_CENTER(4), sonic)->goal.mission = 3; // 4 -  pipe

    Actor * hiddenGoal = newGoalHiddenInactive(tileset_fpgs[0], TILE_CENTER(65), TILE_CENTER(24), sonic); // 5 - behind bushes
    hiddenGoal->goal.mission = 4; // 5 - behind bushes
    fallToBackgroundScript = newFallToBackgroundScript(&playerEventHandler, hiddenGoal, bgaTileMap);

    newGoalHiddenInactive(tileset_fpgs[0], TILE_CENTER(176), TILE_CENTER(25), sonic)->goal.mission = 5; // 6 - Gotta go fast
    //spawnGoal(174, 21);

    newCamera(sonic, FIX32(40), FIX32(128));

    //Actor * tileShader = newTileShader(&bgaTileMap);
}

static void unloadLevel() {
    HGL_ACTOR_deleteAll();
    HGL_COMMAND_deleteAll();
    remove_Particles();
    HGL_free((void*)bgaTileMap.map);
    HGL_free((void*)collisionTileMap.map);
}

static void stateLoadMenu() {
    initCourseMenu(&courseMenu, &gameData.course[0], &gameState);
    loadCourseMenu(&courseMenu);
    GameStateMachine.setMenu();
}

static void stateMenu() {
    //drawTestMenu();
    updateCourseMenuInput(&courseMenu);
    drawCourseMenu(&courseMenu);

    if (buttonState.just_pressed & PAD_START) {
        GameStateMachine.setLoadLevel();
        HGL_ACTOR_deleteAll();
    }
}

static void stateLoadLevel() {
    loadLevel();
    GameStateMachine.setGame();
}

static void stateUnloadLevel() {
    unloadLevel();
    GameStateMachine.setLoadMenu();
}

static void stateGame() {
    if (buttonState.just_pressed & PAD_START) {
        GameStateMachine.setUnloadLevel();
    }

    sonic->sonic.inputHandler.handleInput(&buttonState);
    fallToBackgroundScript->inputHandler.handleInput(&buttonState);

    HGL_COMMAND_updateAll();
    HGL_ANIM_updateAll();

    bgbx = camposx;
    bgby = camposy;

    HGL_ENT_updateAll();
    HGL_ACTOR_updateAll();

    //Handle collisions
    checkCoin(&bgaTileMap, sonic);
    //checkGoal( goal, sonic);

    HGL_ENT_renderAll(bgbx,bgby);
    HGL_SPR_renderAll();

#ifdef PSX
    draw_all_sprites_basic();
    update_Particles();
#endif
    //draw_tilemap_no_wrap(tiles_fpg, 1, &collisionTileMap, bgbx, bgby, 0); //Front
    draw_tilemap_no_wrap(tileset_fpgs[tilesetAnimationState->currentFrame], 1, &bgaTileMap, bgbx, bgby, 0); //Front

    //draw_tilemap(tiles_fpg, bgb_map, &bgbTileMap, bgbx>>1, bgby>>1, 1); //BK
    //draw_tilemap_no_wrap(tiles_fpg, bgb_map, &bgbTileMap, 0, 0, 1); //BK

    //CTOY
#ifndef PSX
    draw_all_sprites_zorder();
    update_Particles();
#endif

    draw_text8(0, font_atlas, "012345789", 8, 8, 0, -1);
    draw_text8(0, font_atlas, "ABCDEFGHIJLMNOPQRSTUVWXYZ", 8, 16, 0, -1);
    draw_text8(0, font_atlas, "Hello", 8, 24, 0, -1);
}



void goToMainMenu(DelayedCommand * command) {
    int8_t mission = (int8_t)command->data;

    printf("Completed mission %i\n", mission);

    CourseMissionState * courseMissionState = &gameState.courseMissionState[0];

    CourseMissionState_completeMission(courseMissionState, mission);
    CourseMissionState_activateMission(courseMissionState, CourseMissionState_getNextMission(courseMissionState));

    GameStateMachine.setUnloadLevel();
}

int gameMain() {
    printf("gameMain\n");

    initButtonStateInput();

    HGL_init();

    HGL_COMMAND_init();
    HGL_ANIM_init();
    HGL_SPR_init();
    HGL_ENT_init();
    HGL_ACTOR_init();
    
    int system_fpg = 0;

    sonic_fpg = new_fpg();

    tileset_fpgs[0] = new_fpg();
    tileset_fpgs[1] = new_fpg();
    tileset_fpgs[2] = new_fpg();
    tileset_fpgs[3] = new_fpg();

    int tiles_fpg = tileset_fpgs[0];

    enemies_fpg = new_fpg();
    //int tiles2_fpg = new_fpg();

    //int texture64_map = load_map_from_memory(system_fpg, texture64);
    //int girl01_map = load_map_from_memory(girl_fpg, girl01);
    //int girl02_map = load_map_from_memory(girl_fpg, girl02);

    font_atlas = load_atlas(system_fpg, "art/gbs-mono", 8,8,16,14);

    int texture64_map = load_map(system_fpg, "art/texture6");
    //printf("texture64_map %i\n", texture64_map);
    //int girl01_map = load_map(girl_fpg, "art/girl01");
    //int girl02_map = load_map(girl_fpg, "art/girl02");

    int sonic_map = load_atlas(sonic_fpg, "art/sonic", 48, 48, 5, 5);

    ////int bga_map = load_atlas(tiles_fpg, "art/bga", 16, 16, 10, 8);
    ////int bga_map = load_atlas(tiles_fpg, "art/ghz1tile", 8, 8, 26, 25);
    //int bga_map = load_atlas(tiles_fpg, "art/ghz16", 16, 16, 16, 15);
    int bga_map = load_atlas(tileset_fpgs[0], "art/smb3t" , 16, 16, 11, 9);
    load_atlas(tileset_fpgs[1], "art/smb3t2", 16, 16, 11, 9);
    load_atlas(tileset_fpgs[2], "art/smb3t3", 16, 16, 11, 9);
    load_atlas(tileset_fpgs[3], "art/smb3t4", 16, 16, 11, 9);
    
    ANIM(TilesetAnimation, 0, 1, 2, 3)

    tilesetAnimationState = HGL_ANIM_new();
    SetAnimationState(tilesetAnimationState, TilesetAnimation, 8);

    //int bgb_map = load_atlas(tiles_fpg, "art/bgb", 16, 16, 15, 13);
    int enemies_map = load_atlas(enemies_fpg, "art/enemies", 48, 32, 4, 2);

    initGameStateMachine();

    /*while(1)
    {
        GameStateMachine.update();
        HGL_frame();
    }*/

    return 0;
}

int gameUpdate() {
    updateInput();
    GameStateMachine.update();
    HGL_frame();
    return 0;
}
