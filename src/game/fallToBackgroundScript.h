#ifndef FALL_TO_BACKGROUND_SCRIPT_H
#define FALL_TO_BACKGROUND_SCRIPT_H

#include "../core/hgl_actor.h"
#include "sonic.h" //PlayerEventHandler

Actor* newFallToBackgroundScript(PlayerEventHandler *playerEventHandler, Actor* goal, Actor* trigger, TileMap levelTileMap, AABB box);

#endif //_FALL_TO_BACKGROUND_SCRIPT_H
