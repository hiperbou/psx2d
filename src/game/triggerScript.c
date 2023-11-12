#include "triggerScript.h"
#include "../core/hgl_actor.h"
#include "../core/hgl_types.h"
#include "sonic.h" //PlayerEventHandler

static PlayerEventHandler *myPlayerEventHandler;

static void constructor(Actor* actor) {
}

inline static bool playerOnTriggerEnter() {
    fix32 x = myPlayerEventHandler->player->entity->x;
    fix32 y = myPlayerEventHandler->player->entity->y;

    return fix32ToInt(x) > 16 * 7; //7 (start) 174 (goal) 178 (bush)
}

static void update(Actor* actor) {
    if (playerOnTriggerEnter()) {
        actor->trigger.onEnterCallback();
    }
}

Actor* newTriggerScript(PlayerEventHandler *playerEventHandler, void * onEnterCallback) {
    myPlayerEventHandler = playerEventHandler;
    Actor* actor = newActor(0, 0, 0, 0, constructor, update);
    actor->trigger.onEnterCallback = onEnterCallback;
    return actor;
}