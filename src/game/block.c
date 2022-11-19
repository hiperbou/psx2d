#include "hgl.h"
#include "hgl_spr.h"
#include "hgl_ent.h"

#include "enemyupdate.h"
#include "actor.h"
#include "hgl_anim.h"

#include "actors.h"

ANIM(AnimBlock, 96)
ANIM(AnimCoin, 7,7,7,7)
ANIM(AnimScore, 97)

#define BOX_JUMP_HEIGHT        FIX32(10)
#define UP_SPEED        FIX32(2)
#define DOWN_SPEED        FIX32(2)

#define COIN_JUMP_HEIGHT        FIX32(48)
#define COIN_UP_SPEED        FIX32(8)
#define COIN_DOWN_SPEED        FIX32(8)

#define SCORE_JUMP_HEIGHT        FIX32(32)
#define SCORE_UP_SPEED        FIX32(0.8)
#define SCORE_DOWN_SPEED        FIX32(0.8)


static void constructorScore(Actor* actor) {
    setAnimation(actor, AnimScore, 32);
    setZ(actor, 1);
    actor->enemy = (EnemyData) { //Needs initialization or else will get old data
        .min = actor->entity->y - SCORE_JUMP_HEIGHT,
        .max = actor->entity->y,
        .direction = 0
    };
}

static void updateScore(Actor* actor) {
    if(updateUpAndDown(actor, SCORE_UP_SPEED, SCORE_DOWN_SPEED) == UpAndDownState_up) {
        deleteActor(actor);
    }
}

static void constructorCoin(Actor* actor) {
    setAnimation(actor, AnimCoin, 8);
    setZ(actor, 1);
    actor->enemy = (EnemyData) { //Needs initialization or else will get old data
        .min = actor->entity->y - COIN_JUMP_HEIGHT,
        .max = actor->entity->y,
        .direction = 0
    };
}

static void updateCoin(Actor* actor) {
    if (updateUpAndDown(actor, COIN_UP_SPEED, COIN_DOWN_SPEED) == UpAndDownState_down) {
        newBlock(2, 1, actor->entity->x, actor->entity->y, 2);
        deleteActor(actor);
    }
}

static void constructor(Actor* actor) {
    setAnimation(actor, AnimBlock, 32); //Needs animation defaults to Empty animation!
    setZ(actor, 1);
    actor->enemy = (EnemyData) { //Needs initialization or else will get old data
        .min = actor->entity->y - BOX_JUMP_HEIGHT,
        .max = actor->entity->y,
        .direction = 0
    };

    newBlock(2, 1, actor->entity->x, actor->entity->y - FIX32(16), 1);
}

static void update(Actor* actor) {
    if (updateUpAndDown(actor, UP_SPEED, DOWN_SPEED) == UpAndDownState_down) {
        deleteActor(actor);
    }
}

typedef struct ActorCallbacks {
    ActorConstructorCallback *constructor;
    ActorUpdateCallback* update;
}ActorCallbacks;

struct ActorCallbacks boxCallbacks[] = {
        { constructor, update },
        { constructorCoin, updateCoin },
        { constructorScore, updateScore }
};

Actor* newBlock(int file, int graph, const fix32 x, const fix32 y, int type) {
    ActorCallbacks* callbacks = &boxCallbacks[type];
	return newActor(file,graph, x, y, callbacks->constructor, callbacks->update);
}

