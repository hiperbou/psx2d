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

static void constructorScore(Actor* actor) {
    setAnimation(actor, AnimScore, 32);
    setZ(actor, 1);
    actor->physics = (PhysicsObjectData) {
        .speed = -FIX32(1.8),
        .gravity = FIX32(0.05),
        .groundY = actor->entity->y
    };
}

static void updateScore(Actor* actor) {
    if(updatePhysicsObject(actor) == UpAndDownState_up) {
        deleteActor(actor);
    }
}

static void constructorCoin(Actor* actor) {
    setAnimation(actor, AnimCoin, 8);
    setZ(actor, 1);
    actor->physics = (PhysicsObjectData) {
        .speed = -FIX32(9.5),
        .gravity = FIX32(1),
        .groundY = actor->entity->y
    };
}

static void updateCoin(Actor* actor) {
    if (updatePhysicsObject(actor) == UpAndDownState_down) {
        newBlock(2, 1, actor->entity->x, actor->entity->y, 2);
        deleteActor(actor);
    }
}

static void constructor(Actor* actor) {
    setAnimation(actor, AnimBlock, 32); //Needs animation defaults to Empty animation!
    setZ(actor, 1);
    actor->physics = (PhysicsObjectData) {
        .speed = -FIX32(4.5),
        .gravity = FIX32(1),
        .groundY = actor->entity->y
    };

    newBlock(2, 1, actor->entity->x, actor->entity->y - FIX32(16), 1);
}

static void update(Actor* actor) {
    if (updatePhysicsObject(actor) == UpAndDownState_down) {
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

