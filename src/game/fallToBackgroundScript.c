#include "fallToBackgroundScript.h"

#include <stdio.h>

#include "../core/hgl_actor.h"
#include "sonic.h" //PlayerEventHandler
#include "../input/buttonstate.h"
#include "../input/input.h"
#include "enemyupdate.h" //setZ
#include "triggerScript.h" //onTriggerEnter

static PlayerEventHandler *myPlayerEventHandler;
static Actor* myGoal;
static Actor* myTrigger;
static TileMap levelTileMap;
static AABB myBox;

static u16 input = 0;

static void fallToBackgroundHandleInput(ButtonState *buttonState){
    input = buttonState->btn;
}

#define BUTTON_DOWN PAD_DOWN

#define TIME_TO_ACTIVATE (60 * 5) //6?
#define TIME_TO_RESTORE  (60 * 10)
static int activateCounter = 0;
static int restoreCounter = 0;

static void constructor(Actor* actor) {
    InputHandlerData* inputHandler = &actor->inputHandler;
    inputHandler->handleInput = fallToBackgroundHandleInput;

    activateCounter = 0;
    restoreCounter = -1;
}

inline static Tile getTileInfoFromMap(fix32 x, fix32 y) {
    return getTileInfo(&levelTileMap, fix32ToInt(x) >> 4, fix32ToInt(y) >> 4);
}

inline static bool playerIsVisible() {
    fix32 x = myPlayerEventHandler->player->entity->x;
    fix32 y = myPlayerEventHandler->player->entity->y;
    return getTileInfoFromMap(x, y).id <= 1;
}

inline static bool playerCrouchingInWhiteBlock() {
    return (input & BUTTON_DOWN) && onTriggerEnter(myPlayerEventHandler->player->entity, &myBox);
}

static void update(Actor* actor) {
    if (restoreCounter > 0) {
        restoreCounter--;
    } else if (restoreCounter == 0 && playerIsVisible()) {
        restoreCounter--;
        setZ(myPlayerEventHandler->player, 0); //TODO: this should be a call fo a doBringToForeground function
        myGoal->goal.deactivate(myGoal);
        myTrigger->trigger.active = false;
    }

    if (playerCrouchingInWhiteBlock()) {
        activateCounter++;
        if(activateCounter == TIME_TO_ACTIVATE) {
            activateCounter = 0;
            restoreCounter = TIME_TO_RESTORE;
            myPlayerEventHandler->player->sonic.doFallToBackground();
            myGoal->goal.activate(myGoal);
            myTrigger->trigger.active = true;
        }
    } else {
        activateCounter = 0;
    }
}

Actor* newFallToBackgroundScript(PlayerEventHandler *playerEventHandler, Actor* goal, Actor* trigger, TileMap tileMap, AABB box) {
    myPlayerEventHandler = playerEventHandler;
    myGoal = goal;
    myTrigger = trigger;
    myBox = box;
    levelTileMap = tileMap;
    return newActor(0, 0, 0, 0, constructor, update);
}