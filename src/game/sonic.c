#include <stdbool.h>
#include "sonic.h"

#include "../core/hgl.h"
#include "../input/buttonstate.h"
#include "../input/input.h"
#include "../engine/sprites.h"
#include "../core/hgl_types.h"
#include "enemyupdate.h"
#include <stdlib.h> //abs
//#include "abs.h"
#include "../engine/fsm.h"

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

#define JUMP_WIN_SPEED  FIX32(-1)
#define GRAVITY_WIN     FIX32(0.025)

#define MIN_ROLLING_ON_FLOOR_SPEED       FIX32(0.001)
#define ROLLING_ON_FLOOR_FRICTION_SPEED  FIX32(0.0234375)
#define ROLLING_ON_FLOOR_DECELERATION_SPEED  FIX32(0.125)
#define ROLLING_ON_FLOOR_Y_OFFSET  FIX32(6)
#define ROLLING_ON_FLOOR_BREAK_SPEED  FIX32(4.5)

#define SENSOR_VERTICAL_WIDTH  FIX32(9)
#define SENSOR_VERTICAL_CEILING_LENGTH  FIX32(10)
#define SENSOR_VERTICAL_GROUND_LENGTH  FIX32(16)

#define SENSOR_HORIZONTAL_LENGTH  FIX32(10)

#define TILE_SIZE_FIX32 FIX32(16)

#define TILE_MASK_SOLID 0x80           //1000 0000
#define TILE_MASK_ONE_WAY 0x40         //0100 0000
#define TILE_MASK_BREAKABLE 0x20       //0010 0000
#define TILE_MASK_FLOOR 0xC0           //1100 0000
#define TILE_MASK_SOLID_BREAKABLE 0xA0 //1010 0000

#define JUMP_BUFFER_FRAMES 10
#define COYOTE_TIME_FRAMES 3

static fix32 posx = FIX32(128);
static fix32 posy = FIX32(128);
static fix32 speedX = FIX32(0);
static fix32 speedY = FIX32(0);

static fix32 posyOffset = 0;
static bool rollingOnFloor = false;

static TileMap collisionTileMap;
static PlayerEventHandler *playerEventHandler;

inline static void setRollingOnFloor(bool enabled) {
    rollingOnFloor = enabled;
    posyOffset = enabled ? ROLLING_ON_FLOOR_Y_OFFSET : 0;
}

inline static fix32 tilePosToWorldPos(int tileXorY) {
    return FIX32(tileXorY << 4);
}

inline static fix32 getGroundTileY(Tile tile) {
    return tilePosToWorldPos(tile.tileY - 1);
}

inline static Tile getTileInfoFromCollisionMap(fix32 x, fix32 y) {
    return getTileInfo(&collisionTileMap, fix32ToInt(x) >> 4, fix32ToInt(y) >> 4);
}

inline static fix32 getGroundCollisionYOrOriginalY(Tile tile, fix32 y, fix32 sensorHeight) {
    if (tile.id & TILE_MASK_FLOOR) {
        return getGroundTileY(tile);
    }
    return y + 1;
}

static fix32 getGroundXY(fix32 x, fix32 y) {
    Tile tile = getTileInfoFromCollisionMap(x, y);
    if (tile.id & TILE_MASK_FLOOR) {
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
    if (tile.id & TILE_MASK_SOLID) {
        return getCeilingTileY(tile, sensorHeight);
    }
    return y;
}

static fix32 getCeiling(fix32 x, fix32 y, fix32 sensorHeight) {
    Tile tile = getCeilingTile(x, y, sensorHeight);
    return getCeilingCollisionYOrOriginalY(tile, y, sensorHeight);
}

inline static fix32 getWallRightX(fix32 x, Tile tile, fix32 sensorWidth) {
    if (tile.id & TILE_MASK_SOLID) {
        return tilePosToWorldPos(tile.tileX) - sensorWidth;
    }
    return x;
}

inline static fix32 getWallLeftX(fix32 x, Tile tile, fix32 sensorWidth) {
    if (tile.id & TILE_MASK_SOLID) {
        return tilePosToWorldPos(tile.tileX + 1) + sensorWidth;
    }
    return x;
}

inline static Tile getTileWallRight(fix32 x, fix32 y, fix32 sensorWidth) {
    return getTileInfoFromCollisionMap(x + sensorWidth, y);
}

inline static Tile getTileWallLeft(fix32 x, fix32 y, fix32 sensorWidth) {
    return getTileInfoFromCollisionMap(x - sensorWidth, y);
}

static fix32 getWallRight(fix32 x, fix32 y, fix32 sensorWidth) {
    Tile tile = getTileWallRight(x, y, sensorWidth);
    return getWallRightX(x, tile, sensorWidth);
}
static fix32 getWallLeft(fix32 x, fix32 y, fix32 sensorWidth) {
    Tile tile = getTileWallLeft(x, y, sensorWidth);
    return getWallLeftX(x, tile, sensorWidth);
}

inline static fix32 checkGroundY(fix32 sensorWidth, fix32 sensorHeight) {
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

static void handleInput(ButtonState* buttonState){
    just_pressed = buttonState->just_pressed;
    released = buttonState->released;
    input = buttonState->btn;
}

#define BUTTON_A PAD_CROSS
#define BUTTON_B PAD_CROSS
#define BUTTON_C PAD_CROSS
#define BUTTON_LEFT PAD_LEFT
#define BUTTON_RIGHT PAD_RIGHT
#define BUTTON_UP PAD_UP
#define BUTTON_DOWN PAD_DOWN
#define BUTTON_NOCLIP PAD_SQUARE
#define BUTTON_WIN PAD_TRIANGLE
#define FALSE (0)
#define TRUE (!FALSE)

CREATE_STATE_MACHINE(StateMachine, Grounded, Jumping, FallingOffLedge, FallToBackground, GoalReached, WinEnter, Win, WinExit, PipeDown, PipeDownEnter)

#include "../engine/actor_fsm.h"
ACTOR_CREATE_STATE_MACHINE(AnimStateMachine, AnimNormal, AnimWin, AnimPipe)

static AnimStateMachine animStateMachine;

static void updateMovementDefault() {
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

static void updateMovementRollingOnFloor() {
    if (input & BUTTON_RIGHT && !(input & BUTTON_LEFT))
    {
        // going opposite side, quick breaking
        if (speedX < 0) speedX += ROLLING_ON_FLOOR_DECELERATION_SPEED;

        if (speedX >= MAX_SPEED) speedX = MAX_SPEED;
    }
    else if (input & BUTTON_LEFT && !(input & BUTTON_RIGHT))
    {
        // going opposite side, quick breaking
        if (speedX > 0) speedX -= ROLLING_ON_FLOOR_DECELERATION_SPEED;

        if (speedX <= -MAX_SPEED) speedX = -MAX_SPEED;
    }

    if (speedX < 0 && speedX < -ROLLING_ON_FLOOR_FRICTION_SPEED) {
        speedX += ROLLING_ON_FLOOR_FRICTION_SPEED;
    } else if (speedX > 0 && speedX > ROLLING_ON_FLOOR_FRICTION_SPEED) {
        speedX -= ROLLING_ON_FLOOR_FRICTION_SPEED;
    } else {
        speedX = 0;
        if (isGrounded()) setRollingOnFloor(false);
    }

    posx += speedX;
    posy += speedY;
}

inline static void updateMovement() {
    if (rollingOnFloor) {
        updateMovementRollingOnFloor();
    } else {
        updateMovementDefault();
    }
}

static int jumpBuffer = JUMP_BUFFER_FRAMES;
static int coyoteTime = 0;

inline static void jump(fix32 jumpSpeed){
    jumpBuffer = 0;
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

inline static void doFallToBackground() {
    setZ(playerEventHandler->player, 7);
    posy = posy + FIX32(1);
    speedY = GRAVITY;
    StateMachine.setFallToBackground();
}

inline static void checkWallsDefault(){
    if((input & BUTTON_NOCLIP)) return;

    if (speedX > 0) {
        fix32 wallX = getWallRight(posx, posy, SENSOR_HORIZONTAL_LENGTH);
        if (posx > wallX) speedX = 0;
        posx = MIN(posx, wallX);
    }
    if (speedX < 0) {
        fix32 wallX = getWallLeft(posx, posy,  SENSOR_HORIZONTAL_LENGTH);
        if (posx < wallX) speedX = 0;
        posx = MAX(posx, wallX);
    }
}

inline static void checkWallsRolling(){
    if((input & BUTTON_NOCLIP)) return;

    if (speedX > 0) {
        Tile tile = getTileWallRight(posx, posy, SENSOR_HORIZONTAL_LENGTH);
        fix32 wallX = getWallRightX(posx, tile, SENSOR_HORIZONTAL_LENGTH);
        if (posx > wallX) {
            if (speedX >= ROLLING_ON_FLOOR_BREAK_SPEED) {
                playerEventHandler->onColidedWithCeilingTile(playerEventHandler, tile);
                playerEventHandler->onColidedWithCeilingTile(playerEventHandler, getTileWallRight(posx, posy - TILE_SIZE_FIX32 , SENSOR_HORIZONTAL_LENGTH));
            } else {
                speedX = 0;
            }
            if(!(tile.id & TILE_MASK_BREAKABLE)) speedX = 0;
        }
        posx = MIN(posx, wallX);
    }
    if (speedX < 0) {
        Tile tile = getTileWallLeft(posx, posy, SENSOR_HORIZONTAL_LENGTH);
        fix32 wallX = getWallLeftX(posx, tile, SENSOR_HORIZONTAL_LENGTH);
        if (posx < wallX) {
            if (speedX <= -ROLLING_ON_FLOOR_BREAK_SPEED) {
                playerEventHandler->onColidedWithCeilingTile(playerEventHandler, tile);
                playerEventHandler->onColidedWithCeilingTile(playerEventHandler, getTileWallLeft(posx, posy - TILE_SIZE_FIX32 , SENSOR_HORIZONTAL_LENGTH));
            } else {
                speedX = 0;
            }
            if(!(tile.id & TILE_MASK_BREAKABLE)) speedX = 0;
        }
        posx = MAX(posx, wallX);
    }
}

inline static void checkWalls() {
    if (rollingOnFloor) {
        checkWallsRolling();
    } else {
        checkWallsDefault();
    }
}

inline static void checkCeilings() {
    if((input & BUTTON_NOCLIP)) return;

    if(speedY<0) {
        Tile tile = checkCeilingTile(SENSOR_VERTICAL_WIDTH, SENSOR_VERTICAL_CEILING_LENGTH);
        if (tile.id & TILE_MASK_SOLID) { //TODO: this doesn leave jump through because jump through are == 3 :S
            int ceilingY = tilePosToWorldPos(tile.tileY + 1) + SENSOR_VERTICAL_CEILING_LENGTH;
            if (posy < ceilingY) {
                playerEventHandler->onColidedWithCeilingTile(playerEventHandler, tile);
                speedY = 0;
            }
            posy = MAX(posy, ceilingY);
        }
    }
}

static void checkGroundOnAir() {
    fix32 groundY = checkGroundY(SENSOR_VERTICAL_WIDTH, SENSOR_VERTICAL_GROUND_LENGTH);
    if (speedY > 0 && posy >= groundY) {
        Tile tile = checkGroundTile(SENSOR_VERTICAL_WIDTH, SENSOR_VERTICAL_GROUND_LENGTH);

        bool processed = playerEventHandler->onGrounded(playerEventHandler, tile);
        if (!processed && StateMachine.isJumping()) {
            processed = playerEventHandler->onColidedWithFloorTile(playerEventHandler, tile);
        }

        if (!processed) {
            posy = groundY;
            speedY = 0;
            setRollingOnFloor(false);
            StateMachine.setGrounded();
        }
    } else {
        speedY += GRAVITY;
    }
}

static void checkJumpBuffer() {
    if (just_pressed & (BUTTON_A | BUTTON_B | BUTTON_C)) {
        jumpBuffer = JUMP_BUFFER_FRAMES;
    } else if (jumpBuffer) {
        jumpBuffer--;
    }
}

static void checkCoyoteTime() {
    if (coyoteTime && just_pressed & (BUTTON_A | BUTTON_B | BUTTON_C)) {
        jump(JUMP_SPEED);
    } else if(coyoteTime) {
        coyoteTime--;
        speedY = 0;
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
    checkJumpBuffer();
}

static void stateFallingOffLedge() {
    updateMovement();
    checkWalls();
    checkCeilings();
    checkGroundOnAir();
    checkCoyoteTime();
    checkJumpBuffer();
}

static void stateFallToBackground() {
    posy += speedY;
    speedY += GRAVITY;
    if(speedY > FIX32(4)) {
        StateMachine.setFallingOffLedge();
    }
}

static void stateGrounded() {
    if (just_pressed & (BUTTON_A | BUTTON_B | BUTTON_C) || (jumpBuffer && (input & (BUTTON_A | BUTTON_B | BUTTON_C)) )) {
        jump(JUMP_SPEED);
        return;
    }

    if (!rollingOnFloor && (input & (BUTTON_DOWN) && !(input & BUTTON_LEFT  || input & BUTTON_RIGHT)) && abs(speedX) >= MIN_ROLLING_ON_FLOOR_SPEED) { //MEGADRIVE(Genesis)
    //if (just_pressed & (BUTTON_DOWN) && abs(speedX) >= MIN_ROLLING_ON_FLOOR_SPEED) { //SMS
        setRollingOnFloor(true);
    }

    updateMovement();

    int groundY = checkGroundY(SENSOR_VERTICAL_WIDTH, SENSOR_VERTICAL_GROUND_LENGTH);
    int groundYStep = getGroundXY(posx, posy);

    if (groundY > posy + SENSOR_VERTICAL_GROUND_LENGTH) {
        speedY = 0;//GRAVITY;
        coyoteTime = COYOTE_TIME_FRAMES;
        StateMachine.setFallingOffLedge();
    } else if(groundYStep<groundY) {
        posy = groundYStep;
    }

    checkWalls();
}

static int8_t missionCompleted = -1;

static void onPlayerReachedGoal(int8_t mission) {
    missionCompleted = mission;
    setAnimWin(&animStateMachine);
    setGoalReached();
}

void doPlayerWinAnimationParticles(Actor*player, int8_t mission); //main.c
static void stateGoalReached() {
    posy += speedY;

    fix32 groundY = checkGroundY(SENSOR_VERTICAL_WIDTH, SENSOR_VERTICAL_GROUND_LENGTH);
    if (speedY > 0 && posy >= groundY) {
        posy = groundY;
        setWinEnter();
        doPlayerWinAnimationParticles(playerEventHandler->player, missionCompleted);
    } else {
        speedY += GRAVITY;
    }
}

static int timer = 0;

static void statePipeDown() {
    //setZ(playerEventHandler->player, 7); //z is changed from somewhere
    posy += FIX32(1);
}

static void statePipeDownEnter() {
    posy += FIX32(1);
    timer++;
    if (timer > 40) {
        timer = 0;
        setZ(playerEventHandler->player, 0);
        setAnimNormal(&animStateMachine);
        setFallingOffLedge();
    }
}

inline static void doPipeDown(fix32 warpPosition) {
    posx = warpPosition;
    setAnimPipe(&animStateMachine);
    setZ(playerEventHandler->player, 7);
    StateMachine.setPipeDown();
}

void sonicDoEnterFromPipe() {
    setAnimPipe(&animStateMachine);
    setZ(playerEventHandler->player, 7);
    StateMachine.setPipeDownEnter();
}


static void stateWinEnter() {
    timer++;
    if (timer > 20) {
        timer = 0;
        posy -= FIX32(4);
        speedY = JUMP_WIN_SPEED;
        setWin();
    }
}

inline static bool debugBreakWinState(){
    if (just_pressed & (BUTTON_A | BUTTON_B | BUTTON_C)) {
        setAnimNormal(&animStateMachine);
        jump(JUMP_SPEED);
        return true;
    }
    return false;
}

static void stateWin() {
    if (debugBreakWinState()) return;

    if (speedY > 0) {
        StateMachine.setWinExit();
    } else {
        posy += speedY;
        speedY += GRAVITY_WIN;
    }
}

static void stateWinExit() {
    if (debugBreakWinState()) return;
}

ANIM(ANIM_STAND, 1)
ANIM(ANIM_WAIT, 21, 22)
ANIM(ANIM_WALK, 2, 3, 4, 5, 6, 7)
ANIM(ANIM_RUN, 8, 9, 10, 11)
ANIM(ANIM_BRAKE, 12, 13)
ANIM(ANIM_UP, 14)
ANIM(ANIM_CROUCH, 15)
ANIM(ANIM_ROLL, 16, 17, 18, 19, 20)
ANIM(ANIM_WIN_ENTER, 23)
ANIM(ANIM_WIN, 24, 25, 25, 25, 25, 25)
ANIM(ANIM_WIN_LOOP, 25)

static void updateAnim(Actor * actor)
{
    if (StateMachine.isJumping() || rollingOnFloor) {
        setAnimation(actor, ANIM_ROLL, 4);
        setAnimationDelay(actor, 1 * (fix32ToInt(MAX_SPEED - abs(speedX))));
    } else {
        if (((speedX >= BRAKE_SPEED) && (input & BUTTON_LEFT)) || ((speedX <= -BRAKE_SPEED) && (input & BUTTON_RIGHT)))
            setAnimation(actor, ANIM_BRAKE, 4);
        else if ((speedX >= RUN_SPEED) || (speedX <= -RUN_SPEED))
            setAnimation(actor, ANIM_RUN, 4);
        else if (speedX != 0 || ((input & BUTTON_LEFT) || (input & BUTTON_RIGHT)) || StateMachine.isFallingOffLedge()) {
            setAnimation(actor, ANIM_WALK, 4);
            setAnimationDelay(actor, 2 * (fix32ToInt(RUN_SPEED - abs(speedX))));
        }
        else
        {
            if (input & BUTTON_UP)
                setAnimation(actor, ANIM_UP, 4);
            else if (input & BUTTON_DOWN)
                setAnimation(actor, ANIM_CROUCH, 4);
            else
                setAnimation(actor, ANIM_STAND, 4);
        }
    }

    if (speedX > 0)
        HGL_SPR_setHFlip(actor->entity->spr, FALSE);
    else if (speedX < 0)
        HGL_SPR_setHFlip(actor->entity->spr, TRUE);
    else if ((input & BUTTON_RIGHT))
        HGL_SPR_setHFlip(actor->entity->spr, FALSE);
    else if ((input & BUTTON_LEFT))
        HGL_SPR_setHFlip(actor->entity->spr, TRUE);

}

static void updateWinAnim(Actor * actor)
{
    if (StateMachine.isWinEnter()) {
        setAnimation(actor, ANIM_WIN_ENTER, 8);
    } else if (StateMachine.isWin()) {
        setAnimation(actor, ANIM_WIN, 8);
    } else if (StateMachine.isWinExit()) {
        setAnimation(actor, ANIM_WIN_LOOP, 8);
    }
}

static void updatePipeAnim(Actor *actor) {
    setAnimation(actor, ANIM_WAIT, 60);
}

static void stateAnimNormal(AnimStateMachine * sm, Actor *actor) {
    updateAnim(actor);
}

static void stateAnimWin(AnimStateMachine * sm, Actor *actor) {
    updateWinAnim(actor);
}

static void stateAnimPipe(AnimStateMachine * sm, Actor *actor) {
    updatePipeAnim(actor);
}

static void update(Actor* actor) {
    StateMachine.update();
    HGL_ENT_setPosition(actor->entity, posx , posy + posyOffset);
    updateAnimStateMachine(&animStateMachine, actor);
}

static void constructor(Actor* actor) {
    posx = actor->entity->x;
    posy = actor->entity->y;
    speedX = FIX32(0);
    speedY = FIX32(0);
    input = 0;
    timer = 0;

    SonicData* sonic = &actor->sonic;
    sonic->inputHandler.handleInput = handleInput;
    sonic->doRebound = doRebound;
    sonic->onPlayerReachedGoal = onPlayerReachedGoal;
    sonic->doFallToBackground = doFallToBackground;
    sonic->doPipeDown = doPipeDown;

    setAnimation(actor, ANIM_STAND, 100);

    setZ(actor, 0);
    setRollingOnFloor(false);

    initStateMachine();
    initAnimStateMachine(&animStateMachine);

    //HGL_SPR_setPalette(actor->entity->spr, PAL0);
}

Actor* newSonic(int file, const fix32 x, const fix32 y, TileMap _collisionTileMap, PlayerEventHandler *_playerEventHandler){
    collisionTileMap = _collisionTileMap;
    playerEventHandler = _playerEventHandler;
    return newActor(file, 1, x, y, constructor, update);
}
