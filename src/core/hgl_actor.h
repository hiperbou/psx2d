#ifndef __HGL_ACTOR_H
#define __HGL_ACTOR_H

#include <stdbool.h>
#include "hgl_ent.h"
#include "../input/buttonstate.h"

#define fun(X) void (*X)(void)
#define fun2(X,Y) void (*X)(Y)
#define boolFun(X) bool (*X)(void)

typedef struct Actor Actor;

typedef struct{
	s16 direction;
    s16 min;
    s16 max;
}EnemyData;

typedef struct{
    fix32 speedY;
    fix32 groundY;
    fix32 gravity;
}PhysicsObjectData;

typedef struct {
    PhysicsObjectData physicsObjectData;
    Actor* targetActor;
    void * sm;
}GoalData;

typedef struct {
    void(*activate)(Actor*);
    void(*deactivate)(Actor*);
    void(*select)(Actor*);
    void(*unselect)(Actor*);
    const int *anim_selected;
    const int *anim_unselected;
}MenuStarData;

typedef struct{
	fun2 (handleInput, ButtonState*);
    fun  (doRebound);
    fun  (onPlayerReachedGoal);
    fun  (doFallToBackground);
}SonicData;

typedef void ActorConstructorCallback(Actor* actor);
typedef void ActorUpdateCallback(Actor* actor);
typedef void ActorDestructorCallback(Actor* actor);

typedef struct Actor {
	HGL_Entity *entity;
    AnimationState *animationState;
	union {
		EnemyData enemy;
        PhysicsObjectData physics;
		SonicData sonic;
        GoalData goal;
        MenuStarData menuStar;
	};
    ActorUpdateCallback *updateCallback;
    ActorDestructorCallback *destructorCallback;
}Actor;

void        HGL_ACTOR_init();
void        HGL_ACTOR_updateAll();
void        HGL_ACTOR_deleteAll();

Actor* newActor(int file, int graph, fix32 x, fix32 y, ActorConstructorCallback* constructorCB, ActorUpdateCallback* updateCB);
Actor* newActorWithDestructor(int file, int graph, fix32 x, fix32 y, ActorConstructorCallback* constructorCB, ActorUpdateCallback* updateCB, ActorDestructorCallback* destructorCB);
void deleteActor(Actor *actor);

#endif //__HGL_ACTOR_H
