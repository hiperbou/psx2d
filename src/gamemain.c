#include "core/hgl.h"
//#include "system.h"
#include "input/input.h"
#include "engine/sprites.h"
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
#include <stdint.h>
#include <stdbool.h>

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
#define TILE(X) FIX32(X*TILE_SIZE)
#define TILE_CENTER(X) FIX32(X*TILE_SIZE + HALF_TILE_SIZE)
#define TILE_X_TO_SCREEN(X) ((X*TILE_SIZE) - camposx)
#define TILE_CENTER_X_TO_SCREEN(X) ((X*TILE_SIZE) + HALF_TILE_SIZE - camposx)
#define TILE_Y_TO_SCREEN(X) ((X*TILE_SIZE) - camposy)
#define TILE_CENTER_Y_TO_SCREEN(X) ((X*TILE_SIZE) + HALF_TILE_SIZE - camposy)
#define POS_TO_TILE_16(X) (fix32ToInt(X)>>4)

#define REPEAT5(X) X; X; X; X; X;
#define REPEAT10(X) REPEAT5(X); REPEAT5(X);
#define REPEAT25(X) REPEAT10(X); REPEAT10(X); REPEAT5(X);
#define REPEAT50(X) REPEAT25(X); REPEAT25(X);
#define REPEAT100(X) REPEAT50(X); REPEAT50(X);


void onPlayerCollidedWithCeilingTile(PlayerEventHandler*playerEventHandler, Tile tile) {
    uint8_t tileId;
    switch (tile.id) {
        case 0: break;
        case 1:
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
                    setTileAt(playerEventHandler->tilemap, tile.tileX, tile.tileY, 0);
                    //spawn block
                    newBlock(2, 1, TILE_CENTER(tile.tileX), TILE_CENTER(tile.tileY), 0);
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


void newGoToMainMenuCommand(int delay, uint8_t *target, uint8_t value) {
    DelayedCommand * command = HGL_COMMAND_new();
    *command = (DelayedCommand) {
            .delay = delay,
            .callback = goToMainMenu,
            .target = target,
            .data = value
    };
}

void doPlayerWinAnimationParticles(Actor*player) {
    int x = TILE_CENTER_X_TO_SCREEN(POS_TO_TILE_16(player->entity->x));
    int y = TILE_CENTER_Y_TO_SCREEN(POS_TO_TILE_16(player->entity->y));
    REPEAT25(new_Particle(x, y))
    newGoToMainMenuCommand(120, NULL, 0);
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
CREATE_STATE_MACHINE(GameStateMachine, Menu, LoadLevel, StartGame, Game, UnloadLevel)


static void drawMenu() {
    //FntPrint("\n\n\n\n\n\n\n\n\t\tPRESS START TO PLAY!\n");
}

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

    fallToBackgroundScript = newFallToBackgroundScript(&playerEventHandler);

    newMotobug(enemies_fpg, TILE(20),TILE(25));
    newMotobug(enemies_fpg, TILE(64),TILE(24));
    newMotobug (enemies_fpg, TILE(90), TILE(25));
    newBee(enemies_fpg, TILE(130),TILE(19));

    //goal = newGoal(tileset_fpgs[0], TILE_CENTER(10), TILE_CENTER(21), sonic);
    //deleteActor(goal);
    newGoal(tileset_fpgs[0], TILE_CENTER(174), TILE_CENTER(21), sonic);
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

static void stateMenu() {
    drawMenu();
    unsigned short btn = getButtons(0);
    if (btn & PAD_START) {
        GameStateMachine.setLoadLevel();
    }
}

static void stateLoadLevel() {
    loadLevel();
    GameStateMachine.setStartGame();
}

static void stateUnloadLevel() {
    unloadLevel();
    GameStateMachine.setMenu();
}

static void stateStartGame() {
    drawMenu();
    unsigned short btn = getButtons(0);
    if (!(btn & PAD_START)) {
        GameStateMachine.setGame();
    }
}

static void stateGame() {
    unsigned short btn = getButtons(0);
    if (btn & PAD_START) {
        GameStateMachine.setUnloadLevel();
    }

    sonic->sonic.handleInput(btn);
    fallToBackgroundScript->sonic.handleInput(btn);

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
}

void goToMainMenu() {
    GameStateMachine.setUnloadLevel();
}


void checkDebugInput() {
    unsigned short btn = getButtons(0);
    if(btn & PAD_UP) {

    }
    else if(btn & PAD_DOWN) {

    }
    if(btn & PAD_LEFT) {

    }
    else if(btn & PAD_RIGHT) {

    }

    if(btn & PAD_CROSS) {

    }
    if(btn & PAD_SQUARE) {

    }
    if (btn & PAD_START) {
        printf("START PRESSED!\n");
    }
}

int gameMain() {
    printf("Hello world demod\n");

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

    int texture64_map = load_map(system_fpg, "art/texture6");
    printf("texture64_map %i\n", texture64_map);
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
    GameStateMachine.update();
    HGL_frame();
    return 0;
}
