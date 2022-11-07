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

static TileMap colisionTilemap;

#define GROUND_HIGH_Z FIX32(MAP_HEIGHT - 48)
#define GROUND_LOW_Z FIX32(MAP_HEIGHT - 40)

//interp = (a*(256 - x) + b*x) >> 8
#define LinearInterpolate(X,Y,Z,UNIT) (X*(UNIT-Z)+Y*Z)
#define LinearInterpolateFIX32(X,Y,Z) ((X)*(FIX32(1)-(Z))+(Y)*(Z))

inline fix32 linearInterpolateFix32(int a, int b, fix32 t) {
    return (a*(FIX32(1)-t)+b*t);
}

static fix32 getGroundXY(fix32 x, fix32 y) {
    Tile tile = getTileInfo(&colisionTilemap, fix32ToInt(x) >> 4, fix32ToInt(y) >> 4);
    if (tile.id > 0) {
        return FIX32( (tile.tileY - 1) << 4 );
    }
    return y + 1; //return ground is down the player
}

static fix32 getCeiling(fix32 x, fix32 y, fix32 sensorHeight) {
    Tile tile = getTileInfo(&colisionTilemap, fix32ToInt(x) >> 4, fix32ToInt(y - sensorHeight) >> 4);
    if (tile.id == 1) {
        return FIX32( (tile.tileY + 1) << 4 ) + sensorHeight;
    }
    return y;
}

static fix32 getWallRight(fix32 x, fix32 y, fix32 sensorWidth) {
    Tile tile = getTileInfo(&colisionTilemap, fix32ToInt(x + sensorWidth) >> 4, fix32ToInt(y) >> 4);
    if (tile.id == 1) {
        return FIX32( (tile.tileX) << 4 ) - sensorWidth;
    }
    return x;
}
static fix32 getWallLeft(fix32 x, fix32 y, fix32 sensorWidth) {
    Tile tile = getTileInfo(&colisionTilemap, fix32ToInt(x - sensorWidth) >> 4, fix32ToInt(y) >> 4);
    if (tile.id == 1) {
        return FIX32( (tile.tileX + 1) << 4 ) + sensorWidth;
    }
    return x;
}


static bool checkGround(fix32 x, fix32 y) {
    int tx = CLAMP(fix32ToInt(x) >> 4, 0, colisionTilemap.numCols - 1);
    int ty = CLAMP(fix32ToInt(y) >> 4, 0, colisionTilemap.numRows - 1);
    return (colisionTilemap.map [tx + ty * colisionTilemap.numCols] > 0);
}

static fix32 getGroundYFake(fix32 x) {
    x = fix32ToInt(x) % MAP_WIDTH;
    if(x < 96) return GROUND_HIGH_Z;
    if(x > 176 && x<400) return GROUND_HIGH_Z;
    if(x > 496) return GROUND_HIGH_Z;
    //x 136 and 456

    //interp = (a*(256 - x) + b*x) >> 8
    if(x<136) {
        int dist = 136 - x;
        int relativePosition = FIX32(dist) / 40;
        return linearInterpolateFix32(fix32ToInt(GROUND_LOW_Z), fix32ToInt(GROUND_HIGH_Z), relativePosition);
    }
    if(x<=176) {
        int dist = 176 - x;
        int relativePosition = FIX32(dist) / 40;
        return linearInterpolateFix32(fix32ToInt(GROUND_HIGH_Z), fix32ToInt(GROUND_LOW_Z), relativePosition);
    }

    return GROUND_LOW_Z;
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

inline static fix32 checkGroundY(fix32 sensorWidth, fix32 sensorHeight){
    //TODO: needs 3 sensors because player width = 1+9+9 and tilewidth=16
    int groundY = getGroundXY(posx, posy + sensorHeight);
    int groundYL = getGroundXY(posx  - sensorWidth, posy + sensorHeight);
    int groundYR = getGroundXY(posx + sensorWidth, posy + sensorHeight);
    return MIN(groundY, MIN(groundYL, groundYR));
}

inline static fix32 checkCeiling(fix32 sensorWidth, fix32 sensorHeight) {
    int wallY = getCeiling(posx, posy, sensorHeight);
    int wallYL = getCeiling(posx - sensorWidth, posy, sensorHeight);
    int wallYR = getCeiling(posx + sensorWidth, posy, sensorHeight);
    return MAX(wallY, MAX(wallYL, wallYR));
}

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

CREATE_STATE_MACHINE(StateMachine, Grounded, Jumping)

inline static void jump(){
    speedY = JUMP_SPEED;
    StateMachine.setJumping();
    //SND_startPlayPCM_XGM(SFX_JUMP, 1, SOUND_PCM_CH2);
}

static void stateJumping() {
    updateMovement();

    if(released & (BUTTON_A | BUTTON_B | BUTTON_C)) {
        if(speedY<JUMP_MIN_SPEED) {
            speedY = JUMP_MIN_SPEED;
        }
    }
    if(!(input & BUTTON_NOCLIP)) {

        if (speedX > 0) {
            int wallX = getWallRight(posx, posy, FIX32(10));
            if (posx > wallX) speedX = 0;
            posx = MIN(posx, wallX);
        }
        if (speedX < 0) {
            int wallX = getWallLeft(posx, posy, FIX32(10));
            if (posx < wallX) speedX = 0;
            posx = MAX(posx, wallX);
        }
        if(speedY<0) {
            int ceilingY = checkCeiling(FIX32(9), FIX32(10));
            if (posy < ceilingY) speedY = 0;
            posy = MAX(posy, ceilingY);
        }
    }

    fix32 groundY = checkGroundY(FIX32(9), FIX32(16));

    if (speedY > 0 && posy >= groundY)
    {
        //printf("Grounded from %i %i\n", fix32ToInt(posy)>>4, fix32ToInt(groundY)>>4);
        //printf("Grounded from %i %i\n", fix32ToInt(posy), fix32ToInt(groundY));
        posy = groundY;
        speedY = 0;
        StateMachine.setGrounded();
    } else {
        speedY += GRAVITY;
        //printf("Falling\n");
    }
}

static void stateGrounded() {
    if(just_pressed & (BUTTON_A | BUTTON_B | BUTTON_C)){
        jump();
        return;
    }

    updateMovement();

    int groundY = checkGroundY(FIX32(9), FIX32(16));;
    int groundYStep = getGroundXY(posx, posy);

    if (groundY > posy + FIX32(16)) {
        speedY = GRAVITY;
        StateMachine.setJumping();
        //printf("Falling from %i %i\n", fix32ToInt(posy)>>4, fix32ToInt(groundY)>>4);
    } else if(groundYStep<groundY) {
        posy = groundYStep;
    }

    if(!(input & BUTTON_NOCLIP)) {
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
        else if (speedX != 0) {
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

    setAnimation(actor, ANIM_STAND, 100);

    setZ(actor, 0);

    initStateMachine();

    //HGL_SPR_setPalette(actor->entity->spr, PAL0);
}

Actor* newSonic(int file, const fix32 x, const fix32 y, TileMap _colisionTilemap){
    colisionTilemap = _colisionTilemap;
    return newActor(file, 1, x, y, constructor, update);
}
