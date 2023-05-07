#include "fallToBackgroundScript.h"
#include "actor.h"
#include "sonic.h" //PlayerEventHandler
#include "input.h"
#include "game/enemyupdate.h" //setZ

static PlayerEventHandler *myPlayerEventHandler;

static u16 input = 0;

static void handleInput(u16 inputState){
    input = inputState;
}

#define BUTTON_DOWN PAD_DOWN

#define TIME_TO_ACTIVATE 60 * 5 //6?
#define TIME_TO_RESTORE  60 * 10
static int activateCounter = 0;
static int restoreCounter = 0;

static void constructor(Actor* actor) {
    SonicData* sonic = &actor->sonic;
    sonic->handleInput = handleInput;

    activateCounter = 0;
    restoreCounter = 0;
}

static void update(Actor* actor) {
    if (restoreCounter > 0) {
        restoreCounter--;
        if (restoreCounter == 0) {
            setZ(myPlayerEventHandler->player, 0);
        }
    }

    if ((input & BUTTON_DOWN)) {
        activateCounter++;
        if(activateCounter == TIME_TO_ACTIVATE) {
            activateCounter = 0;
            restoreCounter = TIME_TO_RESTORE;
            myPlayerEventHandler->player->sonic.doFallToBackground();
        }
    } else {
        activateCounter = 0;
    }
}

Actor* newFallToBackgroundScript(PlayerEventHandler *playerEventHandler) {
    myPlayerEventHandler = playerEventHandler;
    return newActor(0, 0, 0, 0, constructor, update);
}