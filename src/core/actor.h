#ifndef ACTOR_H
#define ACTOR_H

#include "hgl_ent.h"

#define fun(X) void (*X)(void)
#define fun2(X,Y) void (*X)(Y)

typedef struct{
	s16 direction;
}EnemyData;

typedef struct{
	fun2 (handleInput, u16);
}SonicData;

typedef struct Actor{
	HGL_Entity *entity;
	u16 id;
	union {
		EnemyData enemy;
		SonicData sonic;
	};
}Actor;

void        HGL_ACTOR_init();
void        HGL_ACTOR_updateAll();

typedef void _actorConstructorCallback(Actor* actor);
typedef void _actorUpdateCallback(Actor* actor);

Actor* newActor(int file, int graph, fix32 x, fix32 y, _actorConstructorCallback* constructorCB, EntityUpdateCallback* updateCB);
void deleteActor(Actor *actor);

#endif
