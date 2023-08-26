#include "hgl.h"
#include "hgl_spr.h"
#include "hgl_ent.h"

#include "hgl_actor.h"
#include "../pool/ObjectPool.h"

#define MAX_NUM_ACTORS 32

static ObjectPool * actorPool;

void HGL_ACTOR_init() {
    actorPool = new_ObjectPool(MAX_NUM_ACTORS, sizeof(Actor));
}

static Actor* newActorFromPool(){
    PoolElement * elem = ObjectPool_get(actorPool);
    Actor * actor = elem->data;
    return actor;
}

static HGL_Entity* createEntity(int file, int graph, const fix32 x, const fix32 y) {
    //TODO: allow actors without sprite
    HGL_Sprite *spr = HGL_SPR_new(file, graph, fix32ToInt(x), fix32ToInt(y));
    HGL_Entity* entity = HGL_ENT_new(x, y, spr, NULL);
    return entity;
}

Actor* newActor(int file, int graph, const fix32 x, const fix32 y, ActorConstructorCallback* constructorCB, ActorUpdateCallback* updateCB){
	Actor * actor = newActorFromPool();
    if(graph>0) {
        actor->animationState = HGL_ANIM_new();
        actor->animationState->currentFrame = graph;
    } else {
        actor->animationState = NULL;
    }
	actor->entity = createEntity(file, graph, x, y);
    if(updateCB) actor->updateCallback = updateCB;
	if(constructorCB) constructorCB(actor);
    actor->destructorCallback = NULL;
	return actor;
}

Actor* newActorWithDestructor(int file, int graph, fix32 x, fix32 y, ActorConstructorCallback* constructorCB, ActorUpdateCallback* updateCB, ActorDestructorCallback* destructorCB) {
    Actor * actor = newActor(file, graph, x, y ,constructorCB, updateCB);
    actor->destructorCallback = destructorCB;
    return actor;
}

void deleteActor(Actor *actor)
{
    if (actor->destructorCallback) actor->destructorCallback(actor);

    if (actor->entity) HGL_ENT_delete(actor->entity);
    if (actor->animationState) HGL_ANIM_delete(actor->animationState);
    ObjectPool_free(actorPool, actor);
}


inline static void updateActor(Actor* actor) {
    if(actor->updateCallback) actor->updateCallback(actor);

    HGL_Entity * ent = actor->entity;
    if (ent->spr && actor->animationState) {
        ent->spr->graph = actor->animationState->currentFrame;
    }
}

void HGL_ACTOR_updateAll() {
    OBJECTPOOL_ITERATOR_ALLOCATED_START(actorPool, Actor)
        updateActor(it);
    OBJECTPOOL_ITERATOR_ALLOCATED_END
}

void HGL_ACTOR_deleteAll() {
    OBJECTPOOL_ITERATOR_ALLOCATED_START(actorPool, Actor)
            deleteActor(it);
    OBJECTPOOL_ITERATOR_ALLOCATED_END
}


