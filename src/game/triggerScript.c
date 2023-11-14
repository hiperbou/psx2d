#include "triggerScript.h"
#include "../core/hgl_actor.h"
#include "../core/hgl_types.h"
#include "sonic.h" //PlayerEventHandler

static PlayerEventHandler *myPlayerEventHandler;

bool onTriggerEnter(const HGL_Entity* entity, const AABB * box) {
    int x = fix32ToInt(entity->x);
    if (x < box->x || x >box->x + box->w) return false;
    int y = fix32ToInt(entity->y);
    if (y < box->y || y >box->y + box->h) return false;
    return true;
}

static bool playerOnTriggerEnter(const AABB * box) {
    return onTriggerEnter(myPlayerEventHandler->player->entity, box);
}

static void update(Actor* actor) {
    if (!actor->trigger.active) return;

    if (playerOnTriggerEnter(&actor->trigger.box)) {
        actor->trigger.onEnterCallback(actor);
    }
}



Actor* newTriggerScript(PlayerEventHandler *playerEventHandler, TriggerScriptCallback onEnterCallback, bool active, AABB box) {
    myPlayerEventHandler = playerEventHandler;
    Actor* actor = newActor(0, 0, 0, 0, NULL, update);
    actor->trigger = (TriggerData) {
        .onEnterCallback = onEnterCallback,
        .active = active,
        .box = (AABB) {
            .x = box.x,
            .y = box.y,
            .w = box.w,
            .h = box.h
        }
    };
    return actor;
}