#include <stdbool.h>
#include "../core/hgl.h"
#include "../core/hgl_spr.h"
#include "../core/hgl_ent.h"

#include "enemyupdate.h"
#include "../core/hgl_actor.h"
#include "../core/hgl_anim.h"

ANIM(anim_enabled, 98, 99)
ANIM(anim_disabled, 5)
ANIM(anim_selected_enabled,  5, 5, 98, 99)
ANIM(anim_selected_disabled, 0, 0, 5, 5)
ANIM(anim_dead, 0)

static void select(Actor* actor) {
    setAnimation(actor, actor->menuStar.anim_selected, 6);
}

static void unselect(Actor* actor) {
    setAnimation(actor, actor->menuStar.anim_unselected, 6);
}

static void activate(Actor* actor) {
    actor->menuStar.anim_selected = &anim_selected_enabled[0];
    actor->menuStar.anim_unselected = &anim_enabled[0];
    unselect(actor);
}

static void deactivate(Actor* actor) {
    actor->menuStar.anim_selected = &anim_selected_disabled[0];
    actor->menuStar.anim_unselected = &anim_disabled[0];
    unselect(actor);
}

static void constructor(Actor* actor) {
    //setAnimation(actor, anim_disabled, 6);
    setZ(actor, 1);
    actor->menuStar = (MenuStarData) {
        .activate = activate,
        .deactivate = deactivate,
        .select = select,
        .unselect = unselect,
        .anim_selected = &anim_selected_enabled[0],
        .anim_unselected = &anim_enabled[0]
    };
}

Actor* newMenuStar(int file, const fix32 x, const fix32 y) {
    return newActor(file,1, x, y, constructor,NULL);
}

