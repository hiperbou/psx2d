#include <stdbool.h>
#include "sonic.h"

#include "hgl.h"
#include "input.h"
#include "sprites.h"
#include "hgl_types.h"
#include "game/enemyupdate.h"
#include "abs.h"
//#include "hgl_spr.h"
//#include "hgl_ent.h"

//#include "sprite.h"
//#include "sound.h"

//#include "actor.h"
//#include "key.h"


#define SFX_JUMP        64
#define SFX_ROLL        65
#define SFX_STOP        66



#define MAX_SPEED       FIX32(8)
#define RUN_SPEED       FIX32(6)
#define BRAKE_SPEED     FIX32(2)

#define JUMP_SPEED      FIX32(-8)
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
static fix32 movx = FIX32(0);
static fix32 movy = FIX32(0);


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
    int tx = CLAMP(fix32ToInt(x) >> 4, 0, colisionTilemap.numCols - 1);
    int ty = CLAMP(fix32ToInt(y) >> 4, 0, colisionTilemap.numRows - 1);
    if (colisionTilemap.map [tx + ty * colisionTilemap.numCols] > 0) {
        return FIX32( (ty-1) << 4 );
    }
    return y + 16; //return ground is down the player
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

static void jump(){
    if (movy == 0)
    {
        movy = JUMP_SPEED;
        //SND_startPlayPCM_XGM(SFX_JUMP, 1, SOUND_PCM_CH2);
    }
}

static u16 input = 0;
static u16 just_pressed = 0;
static u16 released = 0;

static void handleInput(u16 i){
    just_pressed = (i ^ input) & (~input);
    released = (i ^ input) & (~i);
    input = i;
}

#define BUTTON_A PAD_CROSS
#define BUTTON_B PAD_CROSS
#define BUTTON_C PAD_CROSS
#define BUTTON_LEFT PAD_LEFT
#define BUTTON_RIGHT PAD_RIGHT
#define BUTTON_UP PAD_UP
#define BUTTON_DOWN PAD_DOWN
#define FALSE (0)
#define TRUE (!FALSE)

void updatePhysic(Actor * actor, u16 input)
{
    if(just_pressed & (BUTTON_A | BUTTON_B | BUTTON_C)){
        jump();
    }

    if (input & BUTTON_RIGHT)
    {
        movx += ACCEL;
        // going opposite side, quick breaking
        if (movx < 0) movx += ACCEL;

        if (movx >= MAX_SPEED) movx = MAX_SPEED;
    }
    else if (input & BUTTON_LEFT)
    {
        movx -= ACCEL;
        // going opposite side, quick breaking
        if (movx > 0) movx -= ACCEL;

        if (movx <= -MAX_SPEED) movx = -MAX_SPEED;
    }
    else
    {
        if ((movx < FIX32(0.1)) && (movx > FIX32(-0.1)))
            movx = 0;
        else if ((movx < FIX32(0.3)) && (movx > FIX32(-0.3)))
            movx -= movx >> 2;
        else if ((movx < FIX32(1)) && (movx > FIX32(-1)))
            movx -= movx >> 3;
        else
            movx -= movx >> 4;
    }


    //fix32 posx = ent->x;
    //fix32 posy = ent->y;

    posx += movx;
    posy += movy;

    if (movy)
    {
        int groundY = getGroundXY(posx, posy);
        if (movy>0 && posy >= groundY)
        {
            posy = groundY;
            movy = 0;
            //printf("Grounded\n");
        } else {
            movy += GRAVITY;
            //printf("Falling\n");
        }
    } else {
        int groundY = getGroundXY(posx, posy + FIX32(16));
        int groundYStep = getGroundXY(posx, posy);

        if (groundY > posy + FIX32(16)) {
            //printf("Edge!\n");
            movy = GRAVITY;
        } else if(groundYStep<groundY) {
            posy = groundYStep;
        }
    }

    /*if (posx >= MAX_POSX)
    {
        posx = MAX_POSX;
        movx = 0;
    }
    else if (posx <= MIN_POSX)
    {
        posx = MIN_POSX;
        movx = 0;
    }*/

    // set sprite position
    //HGL_ENT_setPosition(ent, posx , posy);
    HGL_ENT_setPosition(actor->entity, posx , posy);
    //spr->x = posx >> 12;
    //spr->y = posy >> 12;
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
    // jumping
    if (movy) {
        setAnimation(actor, ANIM_ROLL, 4);//HGL_SPR_setAnim(spr, ANIM_ROLL);
        setAnimationDelay(actor, 1 * (fix32ToInt(MAX_SPEED - abs(movx))));
    }
    else
    {
        if (((movx >= BRAKE_SPEED) && (input & BUTTON_LEFT)) || ((movx <= -BRAKE_SPEED) && (input & BUTTON_RIGHT)))
            setAnimation(actor, ANIM_BRAKE, 4);//HGL_SPR_setAnim(spr, ANIM_BRAKE);
        else if ((movx >= RUN_SPEED) || (movx <= -RUN_SPEED))
            setAnimation(actor, ANIM_RUN, 4);//HGL_SPR_setAnim(spr, ANIM_RUN);
        else if (movx != 0) {
            setAnimation(actor, ANIM_WALK, 4);//HGL_SPR_setAnim(spr, ANIM_WALK);
            setAnimationDelay(actor, 2 * (fix32ToInt(RUN_SPEED - abs(movx))));
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

    if (movx > 0)
        HGL_SPR_setHFlip(actor->entity->spr, FALSE);
    else if (movx < 0)
        HGL_SPR_setHFlip(actor->entity->spr, TRUE);

}

static void update(Actor* actor) {
    updatePhysic(actor, input);
    updateAnim(actor);
}

static void constructor(Actor* actor) {
    posx = actor->entity->x;
    posy = actor->entity->y;
    movx = FIX32(0);
    movy = FIX32(0);
    input = 0;

    SonicData* sonic = &actor->sonic;
    sonic->handleInput = handleInput;

    setAnimation(actor, ANIM_STAND, 100);

    setZ(actor, 0);

    //HGL_SPR_setPalette(actor->entity->spr, PAL0);
}

Actor* newSonic(int file, const fix32 x, const fix32 y, TileMap _colisionTilemap){
    colisionTilemap = _colisionTilemap;
    return newActor(file, 1, x, y, constructor, update);
}
