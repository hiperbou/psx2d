#include <stdbool.h>
#include "sonic.h"

#include "hgl.h"
#include "input.h"
#include "sprites.h"
#include "hgl_types.h"
#include "game/enemyupdate.h"
#include "abs.h"
#include "fsm.h"

#define SFX_JUMP        64
#define SFX_ROLL        65
#define SFX_STOP        66

#define MAX_SPEED       FIX32(8)
#define RUN_SPEED       FIX32(6)
#define BRAKE_SPEED     FIX32(2)

#define JUMP_SPEED      FIX32(-8)
#define JUMP_MIN_SPEED  FIX32(-4)
#define GRAVITY         FIX32(0.4)
#define ACCEL           FIX32(0.1)
#define DE_ACCEL        FIX32(0.15)

//#define MIN_POSX        FIX32(10.0)
//#define MAX_POSX        FIX32(400.0)
//#define MAX_POSY        FIX32(156.0)

#define MAP_WIDTH           320 //10240
#define MAP_HEIGHT          416//992//224 //1280

#define MIN_POSX            FIX32(10L)
#define MAX_POSX            FIX32(MAP_WIDTH - 100)
#define MAX_POSY            FIX32(MAP_HEIGHT - 48)

static fix32 posx = FIX32(128);
static fix32 posy = FIX32(128);
static fix32 speedX = FIX32(0);
static fix32 speedY = FIX32(0);

static TileMap collisionTilemap;
static PlayerEventHandler *playerEventHandler;

inline static fix32 tilePosToWorldPos(int tileXorY) {
    return FIX32(tileXorY << 4);
}

inline static fix32 getGroundTileY(Tile tile) {
    return tilePosToWorldPos(tile.tileY - 1);
}

inline static Tile getTileInfoFromCollisionMap(fix32 x, fix32 y) {
    return getTileInfo(&collisionTilemap, fix32ToInt(x) >> 4, fix32ToInt(y) >> 4);
}

inline static fix32 getGroundCollisionYOrOriginalY(Tile tile, fix32 y, fix32 sensorHeight) {
    if (tile.id > 0) {
        return getGroundTileY(tile);
    }
    return y + 1;
}

static fix32 getGroundXY(fix32 x, fix32 y) {
    Tile tile = getTileInfoFromCollisionMap(x, y);
    if (tile.id > 0) {
        return getGroundTileY(tile);
    }
    return y + 1; //return ground is down the player
}

inline static Tile getGroundTile(fix32 x, fix32 y, fix32 sensorHeight) {
    return getTileInfoFromCollisionMap(x, y + sensorHeight);
}

static fix32 getGround(fix32 x, fix32 y, fix32 sensorHeight) {
    Tile tile = getGroundTile(x, y, sensorHeight);
    return getGroundCollisionYOrOriginalY(tile, y, sensorHeight);
}

inline static Tile getCeilingTile(fix32 x, fix32 y, fix32 sensorHeight) {
    return getTileInfoFromCollisionMap(x, y - sensorHeight);
}

inline static fix32 getCeilingTileY(Tile tile, fix32 sensorHeight) {
    return tilePosToWorldPos(tile.tileY + 1) + sensorHeight;
}

inline static fix32 getCeilingCollisionYOrOriginalY(Tile tile, fix32 y, fix32 sensorHeight) {
    if (tile.id == 1) {
        return getCeilingTileY(tile, sensorHeight);
    }
    return y;
}

static fix32 getCeiling(fix32 x, fix32 y, fix32 sensorHeight) {
    Tile tile = getCeilingTile(x, y, sensorHeight);
    return getCeilingCollisionYOrOriginalY(tile, y, sensorHeight);
}

static fix32 getWallRight(fix32 x, fix32 y, fix32 sensorWidth) {
    Tile tile = getTileInfoFromCollisionMap(x + sensorWidth, y);
    if (tile.id == 1) {
        return tilePosToWorldPos(tile.tileX) - sensorWidth;
    }
    return x;
}
static fix32 getWallLeft(fix32 x, fix32 y, fix32 sensorWidth) {
    Tile tile = getTileInfoFromCollisionMap(x - sensorWidth, y);
    if (tile.id == 1) {
        return tilePosToWorldPos(tile.tileX + 1) + sensorWidth;
    }
    return x;
}

inline static fix32 checkGroundY(fix32 sensorWidth, fix32 sensorHeight){
    //TODO: needs 3 sensors because player width = 1+9+9 and tilewidth=16
    int groundY = getGroundXY(posx, posy + sensorHeight);
    int groundYL = getGroundXY(posx  - sensorWidth, posy + sensorHeight);
    int groundYR = getGroundXY(posx + sensorWidth, posy + sensorHeight);
    return MIN(groundY, MIN(groundYL, groundYR));
}

inline static Tile checkGroundTile(fix32 sensorWidth, fix32 sensorHeight) {
    Tile ceiling = getGroundTile(posx, posy, sensorHeight);
    Tile ceilingL = getGroundTile(posx - sensorWidth, posy, sensorHeight);
    Tile ceilingR = getGroundTile(posx + sensorWidth, posy, sensorHeight);
    int ceilY = getCeilingCollisionYOrOriginalY(ceiling, posy, sensorHeight);
    int ceilYL = getCeilingCollisionYOrOriginalY(ceilingL, posy, sensorHeight);
    int ceilYR = getCeilingCollisionYOrOriginalY(ceilingR, posy, sensorHeight);
    int winner = MAX(ceilY, MAX(ceilYL, ceilYR));

    if(winner == ceilY) return ceiling;
    if(winner == ceilYL) return ceilingL;
    return ceilingR;
}

inline static fix32 checkCeiling(fix32 sensorWidth, fix32 sensorHeight) {
    int wallY = getCeiling(posx, posy, sensorHeight);
    int wallYL = getCeiling(posx - sensorWidth, posy, sensorHeight);
    int wallYR = getCeiling(posx + sensorWidth, posy, sensorHeight);
    return MAX(wallY, MAX(wallYL, wallYR));
}

inline static Tile checkCeilingTile(fix32 sensorWidth, fix32 sensorHeight) {
    Tile ceiling = getCeilingTile(posx, posy, sensorHeight);
    Tile ceilingL = getCeilingTile(posx - sensorWidth, posy, sensorHeight);
    Tile ceilingR = getCeilingTile(posx + sensorWidth, posy, sensorHeight);
    int ceilY = getCeilingCollisionYOrOriginalY(ceiling, posy, sensorHeight);
    int ceilYL = getCeilingCollisionYOrOriginalY(ceilingL, posy, sensorHeight);
    int ceilYR = getCeilingCollisionYOrOriginalY(ceilingR, posy, sensorHeight);
    int winner = MAX(ceilY, MAX(ceilYL, ceilYR));

    if(winner == ceilY) return ceiling;
    if(winner == ceilYL) return ceilingL;
    return ceilingR;
}

static u16 input = 0;
static u16 just_pressed = 0;
static u16 released = 0;

static void handleInput(u16 inputState){
    just_pressed = (inputState ^ input) & (~input);
    released = (inputState ^ input) & (~inputState);
    input = inputState;
}

#define BUTTON_A PAD_CROSS
#define BUTTON_B PAD_CROSS
#define BUTTON_C PAD_CROSS
#define BUTTON_LEFT PAD_LEFT
#define BUTTON_RIGHT PAD_RIGHT
#define BUTTON_UP PAD_UP
#define BUTTON_DOWN PAD_DOWN
#define BUTTON_NOCLIP PAD_SQUARE
#define FALSE (0)
#define TRUE (!FALSE)


static void updateMovement() {
    if (input & BUTTON_RIGHT && !(input & BUTTON_LEFT))
    {
        speedX += ACCEL;
        // going opposite side, quick breaking
        if (speedX < 0) speedX += ACCEL;

        if (speedX >= MAX_SPEED) speedX = MAX_SPEED;
    }
    else if (input & BUTTON_LEFT && !(input & BUTTON_RIGHT))
    {
        speedX -= ACCEL;
        // going opposite side, quick breaking
        if (speedX > 0) speedX -= ACCEL;

        if (speedX <= -MAX_SPEED) speedX = -MAX_SPEED;
    }
    else
    {
        if ((speedX < FIX32(0.1)) && (speedX > FIX32(-0.1)))
            speedX = 0;
        else if ((speedX < FIX32(0.3)) && (speedX > FIX32(-0.3)))
            speedX -= speedX >> 2;
        else if ((speedX < FIX32(1)) && (speedX > FIX32(-1)))
            speedX -= speedX >> 3;
        else
            speedX -= speedX >> 4;
    }

    posx += speedX;
    posy += speedY;
}

CREATE_STATE_MACHINE(StateMachine, Grounded, Jumping, FallingOffLedge)

inline static void jump(fix32 jumpSpeed){
    speedY = jumpSpeed;
    StateMachine.setJumping();
    //SND_startPlayPCM_XGM(SFX_JUMP, 1, SOUND_PCM_CH2);
}

inline static void doRebound() {
    if(input & (BUTTON_A | BUTTON_B | BUTTON_C)) {
        jump(-speedY);
    } else {
        jump(JUMP_MIN_SPEED);
    }
}

inline static void checkWalls(){
    if((input & BUTTON_NOCLIP)) return;

    if (speedX > 0) {
        int wallX = getWallRight(posx, posy, FIX32(10));
        if (posx > wallX) speedX = 0;
        posx = MIN(posx, wallX);
    }
    if (speedX < 0) {
        int wallX = getWallLeft(posx, posy,  FIX32(10));
        if (posx < wallX) speedX = 0;
        posx = MAX(posx, wallX);
    }
}

inline static void checkCeilings() {
    if((input & BUTTON_NOCLIP)) return;

    if(speedY<0) {
        Tile tile = checkCeilingTile(FIX32(9), FIX32(10));
        if (tile.id == 1) {
            int ceilingY = tilePosToWorldPos(tile.tileY + 1) + FIX32(10);;
            if (posy < ceilingY) {
                playerEventHandler->onColidedWithCeilingTile(playerEventHandler, tile);
                speedY = 0;
            }
            posy = MAX(posy, ceilingY);
        }
    }
}

static void checkGroundOnAir() {
    fix32 groundY = checkGroundY(FIX32(9), FIX32(16));
    if (speedY > 0 && posy >= groundY) {
        //printf("Grounded from %i %i\n", fix32ToInt(posy)>>4, fix32ToInt(groundY)>>4);
        //printf("Grounded from %i %i\n", fix32ToInt(posy), fix32ToInt(groundY));
        if(StateMachine.isJumping()) {
            Tile tile = checkGroundTile(FIX32(9), FIX32(16));
            bool processed = playerEventHandler->onColidedWithFloorTile(playerEventHandler, tile);

            if (!processed) {
                posy = groundY;
                speedY = 0;
                StateMachine.setGrounded();
            }
        } else {
            posy = groundY;
            speedY = 0;
            StateMachine.setGrounded();
        }
    } else {
        speedY += GRAVITY;
    }
}

static void stateJumping() {
    updateMovement();

    if(released & (BUTTON_A | BUTTON_B | BUTTON_C)) {
        if(speedY<JUMP_MIN_SPEED) {
            speedY = JUMP_MIN_SPEED;
        }
    }

    checkWalls();
    checkCeilings();
    checkGroundOnAir();
}

static void stateFallingOffLedge() {
    updateMovement();
    checkWalls();
    checkCeilings();
    checkGroundOnAir();
}

static void stateGrounded() {
    if (just_pressed & (BUTTON_A | BUTTON_B | BUTTON_C)) {
        jump(JUMP_SPEED);
        return;
    }

    updateMovement();

    int groundY = checkGroundY(FIX32(9), FIX32(16));;
    int groundYStep = getGroundXY(posx, posy);

    if (groundY > posy + FIX32(16)) {
        speedY = GRAVITY;
        StateMachine.setFallingOffLedge();
        //printf("Falling from %i %i\n", fix32ToInt(posy)>>4, fix32ToInt(groundY)>>4);
    } else if(groundYStep<groundY) {
        posy = groundYStep;
    }

    checkWalls();
}

ANIM(ANIM_STAND, 1)
ANIM(ANIM_WAIT, 21, 22)
ANIM(ANIM_WALK, 2, 3, 4, 5, 6, 7)
ANIM(ANIM_RUN, 8, 9, 10, 11)
ANIM(ANIM_BRAKE, 12, 13)
ANIM(ANIM_UP, 14)
ANIM(ANIM_CROUCH, 15)
ANIM(ANIM_ROLL, 16, 17, 18, 19, 20)

static void updateAnim(Actor * actor)
{
    if (StateMachine.isJumping()) {
        setAnimation(actor, ANIM_ROLL, 4);
        setAnimationDelay(actor, 1 * (fix32ToInt(MAX_SPEED - abs(speedX))));
    }
    else
    {
        if (((speedX >= BRAKE_SPEED) && (input & BUTTON_LEFT)) || ((speedX <= -BRAKE_SPEED) && (input & BUTTON_RIGHT)))
            setAnimation(actor, ANIM_BRAKE, 4);//HGL_SPR_setAnim(spr, ANIM_BRAKE);
        else if ((speedX >= RUN_SPEED) || (speedX <= -RUN_SPEED))
            setAnimation(actor, ANIM_RUN, 4);//HGL_SPR_setAnim(spr, ANIM_RUN);
        else if (speedX != 0 || ((input & BUTTON_LEFT) || (input & BUTTON_RIGHT))) {
            setAnimation(actor, ANIM_WALK, 4);//HGL_SPR_setAnim(spr, ANIM_WALK);
            setAnimationDelay(actor, 2 * (fix32ToInt(RUN_SPEED - abs(speedX))));
        }
        else
        {
            if (input & BUTTON_UP)
                setAnimation(actor, ANIM_UP, 4);//HGL_SPR_setAnim(spr, ANIM_UP);
            else if (input & BUTTON_DOWN)
                setAnimation(actor, ANIM_CROUCH, 4);//HGL_SPR_setAnim(spr, ANIM_CROUCH);
            else
                setAnimation(actor, ANIM_STAND, 4);//HGL_SPR_setAnim(spr, ANIM_STAND);
        }
    }

    if (speedX > 0)
        HGL_SPR_setHFlip(actor->entity->spr, FALSE);
    else if (speedX < 0)
        HGL_SPR_setHFlip(actor->entity->spr, TRUE);

}

static void update(Actor* actor) {
    StateMachine.update();
    HGL_ENT_setPosition(actor->entity, posx , posy);
    updateAnim(actor);
}

static void constructor(Actor* actor) {
    posx = actor->entity->x;
    posy = actor->entity->y;
    speedX = FIX32(0);
    speedY = FIX32(0);
    input = 0;

    SonicData* sonic = &actor->sonic;
    sonic->handleInput = handleInput;
    sonic->doRebound = doRebound;

    setAnimation(actor, ANIM_STAND, 100);

    setZ(actor, 0);

    initStateMachine();

    //HGL_SPR_setPalette(actor->entity->spr, PAL0);
}

Actor* newSonic(int file, const fix32 x, const fix32 y, TileMap _collisionTilemap, PlayerEventHandler *_playerEventHandler){
    collisionTilemap = _collisionTilemap;
    playerEventHandler = _playerEventHandler;
    return newActor(file, 1, x, y, constructor, update);
}
