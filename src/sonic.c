#include "sonic.h"

#include "hgl.h"
#include "input.h"
#include "sprites.h"
#include "hgl_types.h"
//#include "hgl_spr.h"
//#include "hgl_ent.h"

//#include "sprite.h"
//#include "sound.h"

//#include "actor.h"
//#include "key.h"


#define SFX_JUMP        64
#define SFX_ROLL        65
#define SFX_STOP        66

#define ANIM_STAND      0
#define ANIM_WAIT       1
#define ANIM_WALK       2
#define ANIM_RUN        3
#define ANIM_BRAKE      4
#define ANIM_UP         5
#define ANIM_CROUCH    6
#define ANIM_ROLL       7

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
#define MAP_HEIGHT          224 //1280

#define MIN_POSX            FIX32(10L)
#define MAX_POSX            FIX32(MAP_WIDTH - 100)
#define MAX_POSY            FIX32(MAP_HEIGHT - 40)

static fix32 posx = FIX32(128);
static fix32 posy = FIX32(128);
static fix32 movx = FIX32(0);
static fix32 movy = FIX32(0);

static void jump(){
    if (movy == 0)
    {
        movy = JUMP_SPEED;
        //SND_startPlayPCM_XGM(SFX_JUMP, 1, SOUND_PCM_CH2);
    }
}

static u16 input = 0;
static void handleInput(u16 i){
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

void updatePhysic(Tsprite * spr/*HGL_Entity *ent*/, u16 input)
{
    if(input & (BUTTON_A | BUTTON_B | BUTTON_C)){
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
        if (posy > MAX_POSY)
        {
            posy = MAX_POSY;
            movy = 0;
            //TEST
            //shot(ent);
            //TEST
        }
        else movy += GRAVITY;
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
    spr->x = posx >> 12;
    spr->y = posy >> 12;
}
/*
static void updateAnim(HGL_Sprite * spr)
{
    // jumping
    if (movy) HGL_SPR_setAnim(spr, ANIM_ROLL);
    else
    {
        if (((movx >= BRAKE_SPEED) && (input & BUTTON_LEFT)) || ((movx <= -BRAKE_SPEED) && (input & BUTTON_RIGHT)))
            HGL_SPR_setAnim(spr, ANIM_BRAKE);
        else if ((movx >= RUN_SPEED) || (movx <= -RUN_SPEED))
            HGL_SPR_setAnim(spr, ANIM_RUN);
        else if (movx != 0)
            HGL_SPR_setAnim(spr, ANIM_WALK);
        else
        {
            if (input & BUTTON_UP)
                HGL_SPR_setAnim(spr, ANIM_UP);
            else if (input & BUTTON_DOWN)
                HGL_SPR_setAnim(spr, ANIM_CROUCH);
            else
                HGL_SPR_setAnim(spr, ANIM_STAND);
        }
    }

    if (movx > 0)
        HGL_SPR_setHFlip(spr, FALSE);
    else if (movx < 0)
        HGL_SPR_setHFlip(spr, TRUE);

}*/
/*
static void update(HGL_Entity* ent) {
    updatePhysic(ent);
    updateAnim(ent->spr);
}*/

/*static void constructor(Actor* actor) {
    posx = actor->entity->x;
    posy = actor->entity->y;
    movx = FIX32(0);
    movy = FIX32(0);
    input = 0;

    SonicData* sonic = &actor->sonic;
    sonic->handleInput = handleInput;

    HGL_SPR_setPalette(actor->entity->spr, PAL0);
}*/

/*Actor* newSonic(const fix32 x, const fix32 y){
    return newActor(x, y, &sprite_sonic, constructor, update);
}
*/