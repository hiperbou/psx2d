#ifndef ACTOR_H
#define ACTOR_H

#include "hgl_ent.h"

#define fun(X) void (*X)(void)
#define fun2(X,Y) void (*X)(Y)

typedef struct{
	s16 direction;
    s16 min;
    s16 max;
}EnemyData;

typedef struct{
	s16 direction;
    s16 min;
    s16 max;
}BlockData;


typedef struct{
    fix32 speed;
    fix32 groundY;
    fix32 gravity;
}PhysicsObjectData;

typedef struct{
	fun2 (handleInput, u16);
    fun (doRebound);
}SonicData;

typedef struct Actor Actor;
typedef void ActorConstructorCallback(Actor* actor);
typedef void ActorUpdateCallback(Actor* actor);

typedef struct Actor {
	HGL_Entity *entity;
    AnimationState *animationState;
	union {
		EnemyData enemy;
        PhysicsObjectData physics;
		SonicData sonic;
	};
    ActorUpdateCallback *updateCallback;
}Actor;

void        HGL_ACTOR_init();
void        HGL_ACTOR_updateAll();
void        HGL_ACTOR_deleteAll();

Actor* newActor(int file, int graph, fix32 x, fix32 y, ActorConstructorCallback* constructorCB, ActorUpdateCallback* updateCB);
void deleteActor(Actor *actor);

#endif
