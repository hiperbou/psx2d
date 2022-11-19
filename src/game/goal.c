#include <stdbool.h>
#include "hgl.h"
#include "hgl_spr.h"
#include "hgl_ent.h"

#include "enemyupdate.h"
#include "actor.h"
#include "hgl_anim.h"

ANIM(anim_idle, 2, 5, 6)
ANIM(anim_activated, 5)
ANIM(anim_dead, 0)

#include "fsm.h"
CREATE_STATE_MACHINE(GoalStateMachine, Idle, Activated, Dead)

Actor* goalActor;
Actor* targetActor;
#define SCORE_JUMP_HEIGHT        FIX32(64)
#define SCORE_UP_SPEED        FIX32(2)
#define SCORE_DOWN_SPEED        FIX32(2)


bool checkColision() {
    if(targetActor->entity->x < goalActor->entity->x - FIX32(16) ||
        targetActor->entity->x > goalActor->entity->x + FIX32(16) ||
        targetActor->entity->y < goalActor->entity->y - FIX32(16) ||
        targetActor->entity->y > goalActor->entity->y + FIX32(16)) return false;
    return true;
}



static void stateIdle() {
    if(checkColision()) {
        GoalStateMachine.setActivated();
    }
}

static void stateActivated() {
    Actor * actor = goalActor;
    setAnimation(actor, anim_activated, 6);

    if(updatePhysicsObject(actor) == UpAndDownState_up) {
        setAnimation(goalActor, anim_dead, 6);
        GoalStateMachine.setDead();
    }
}

static void stateDead() {

}

static void update(Actor* actor) {
    updateGoalStateMachine();
}

static void constructor(Actor* actor) {
    goalActor = actor;
    initGoalStateMachine();
    setAnimation(actor, anim_idle, 6);
    setZ(actor, 1);
    actor->physics = (PhysicsObjectData) {
            .speed = -FIX32(4),
            .gravity = FIX32(0.1),
            .groundY = actor->entity->y
    };
}

Actor* newGoal(int file, const fix32 x, const fix32 y, Actor * target) {
    targetActor = target;
    return newActor(file,1, x, y,constructor,update);
}