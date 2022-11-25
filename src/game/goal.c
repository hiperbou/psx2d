#include <stdbool.h>
#include "hgl.h"
#include "hgl_spr.h"
#include "hgl_ent.h"

#include "enemyupdate.h"
#include "actor.h"
#include "hgl_anim.h"

ANIM(anim_idle, 2, 5, 6)
ANIM(anim_activated, 5)
ANIM(anim_activated2, 98,5,99,5)
ANIM(anim_dead, 0)
ANIM(anim_star, 98,99)

#include "engine/actor_fsm.h"
#include "hgl_mem.h"

ACTOR_CREATE_STATE_MACHINE(GoalStateMachine, Idle, Activated, Star, Float, Dead)

bool checkColision(Actor * actor, Actor * targetActor) {
    if(targetActor->entity->x < actor->entity->x - FIX32(16) ||
       targetActor->entity->x > actor->entity->x + FIX32(16) ||
       targetActor->entity->y < actor->entity->y - FIX32(16) ||
       targetActor->entity->y > actor->entity->y + FIX32(16)) return false;
    return true;
}

static void stateIdle(GoalStateMachine * sm, Actor *actor) {
    if(checkColision(actor, actor->goal.targetActor)) {
        setActivated(sm);
    }
}

static void stateActivated(GoalStateMachine * sm, Actor *actor) {
    setAnimation(actor, anim_activated, 6);

    if(updatePhysicsObject(actor) == UpAndDownState_up) {
        setAnimation(actor, anim_activated2, 3);
        setStar(sm);
    }
}

static void stateStar(GoalStateMachine * sm, Actor *actor) {
    if(updatePhysicsObject(actor) == UpAndDownState_down) {
        setAnimation(actor, anim_star, 6);
        setFloat(sm);
    }
}

static void stateFloat(GoalStateMachine * sm, Actor *actor) {
    if(checkColision(actor, actor->goal.targetActor)) {
        setAnimation(actor, anim_dead, 6);
        setDead(sm);
    }
}

static void stateDead(GoalStateMachine * sm, Actor *actor) {
    deleteActor(actor);
}

static void update(Actor* actor) {
    updateGoalStateMachine(actor->goal.sm, actor);
}

static Actor* targetActor;

static void destructor(Actor *actor) {
    HGL_free(actor->goal.sm);
}

static void constructor(Actor* actor) {
    GoalStateMachine *goalStateMachine = HGL_malloc(sizeof (GoalStateMachine));
    initGoalStateMachine(goalStateMachine);
    setAnimation(actor, anim_idle, 6);
    setZ(actor, 1);
    actor->goal = (GoalData) {
        .physicsObjectData = (PhysicsObjectData) {
                .speedY = -FIX32(4),
                .gravity = FIX32(0.1),
                .groundY = actor->entity->y
        },
        .targetActor = targetActor,
        .sm = goalStateMachine
    };
}

static void constructorActivated(Actor* actor) {
    constructor(actor);
    GoalStateMachine *sm = actor->goal.sm;
    sm->setActivated(sm);
}

Actor* newGoal(int file, const fix32 x, const fix32 y, Actor * target) {
    targetActor = target;
    return newActorWithDestructor(file,1, x, y,constructor,update, destructor);
}

Actor* newGoalActivated(int file, const fix32 x, const fix32 y, Actor * target) {
    targetActor = target;
    return newActorWithDestructor(file,1, x, y,constructorActivated,update, destructor);
}