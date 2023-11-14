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
    int8_t mission;
    void(*activate)(Actor*);
    void(*deactivate)(Actor*);
    bool collisionEnabled;
}GoalData;

typedef struct {
    void(*activate)(Actor*);
    void(*deactivate)(Actor*);
    void(*select)(Actor*);
    void(*unselect)(Actor*);
    const int *anim_selected;
    const int *anim_unselected;
}MenuStarData;

typedef struct {
    fun2 (handleInput, ButtonState*);
}InputHandlerData;

typedef struct{
    InputHandlerData inputHandler;
    fun  (doRebound);
    fun2  (onPlayerReachedGoal, int8_t);
    fun  (doFallToBackground);
    fun2 (doPipeDown, fix32);
}SonicData;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
}AABB;

typedef struct {
    fun2 (onEnterCallback, Actor*);
    bool active;
    AABB box;
}TriggerData;

typedef void ActorConstructorCallback(Actor* actor);
typedef void ActorUpdateCallback(Actor* actor);
typedef void ActorDestructorCallback(Actor* actor);

typedef struct Actor {
	HGL_Entity *entity;
    AnimationState *animationState;
	union {
        InputHandlerData inputHandler;
		EnemyData enemy;
        PhysicsObjectData physics;
		SonicData sonic;
        GoalData goal;
        MenuStarData menuStar;
	    TriggerData trigger;
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
