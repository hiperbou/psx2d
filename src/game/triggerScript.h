#ifndef TRIGGER_SCRIPT_H
#define TRIGGER_SCRIPT_H

#include "../core/hgl_actor.h"
#include "sonic.h" //PlayerEventHandler

typedef void TriggerScriptCallback(Actor* trigger);


bool onTriggerEnter(const HGL_Entity* entity, const AABB * box);

Actor* newTriggerScript(PlayerEventHandler *playerEventHandler, TriggerScriptCallback onEnterCallback, bool active, AABB box);

#endif //_FALL_TO_BACKGROUND_SCRIPT_H
