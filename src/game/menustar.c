#include <stdbool.h>
#include "../core/hgl.h"
#include "../core/hgl_spr.h"
#include "../core/hgl_ent.h"

#include "enemyupdate.h"
#include "../core/hgl_actor.h"
#include "../core/hgl_anim.h"

ANIM(anim_enabled, 98, 99)
ANIM(anim_disabled, 5)
ANIM(anim_selected_enabled, 98, 99, 5, 5)
ANIM(anim_selected_disabled, 5, 5, 0, 0)
ANIM(anim_dead, 0)

#include "../engine/actor_fsm.h"
#include "../core/hgl_mem.h"

ACTOR_CREATE_STATE_MACHINE(MenuStarStateMachine, Idle, Activated)

static void stateIdle(MenuStarStateMachine * sm, Actor *actor) {
    setAnimation(actor, anim_enabled, 6);
}

static void stateActivated(MenuStarStateMachine * sm, Actor *actor) {
    setAnimation(actor, anim_enabled, 6);
}

static void update(Actor* actor) {
    updateMenuStarStateMachine(actor->menuStar.sm, actor);
}

static void destructor(Actor *actor) {
    HGL_free(actor->menuStar.sm);
}

static void constructor(Actor* actor) {
    MenuStarStateMachine *menuStarStateMachine = HGL_malloc(sizeof (MenuStarStateMachine));
    initMenuStarStateMachine(menuStarStateMachine);
    setAnimation(actor, anim_enabled, 6);
    setZ(actor, 1);
    actor->menuStar = (MenuStarData) {
        .sm = menuStarStateMachine
    };
}

inline static Actor* newMenuStarBuilder(int file, const fix32 x, const fix32 y, ActorConstructorCallback* constructorCB) {
    return newActorWithDestructor(file,1, x, y,constructorCB,update, destructor);
}

Actor* newMenuStar(int file, const fix32 x, const fix32 y) {
    return newMenuStarBuilder(file, x, y, constructor);
}

