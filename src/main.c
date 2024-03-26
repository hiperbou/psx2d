#include "core/hgl.h"
#include "game.h"
#include "gameresources.h"

#include "utils/async.h"
//#include "cppfunction.h"

int gameMain();
int gameUpdate();

int gameTitle();
int gameTitleUpdate();

int gameEnding();
int gameEndingUpdate();

async asyncMainLoop(struct async *mainLoopAsyncState) {
    async_begin(mainLoopAsyncState);
    gameInit();
    initResources();
    gameTitle();
    await_while(gameTitleUpdate());
    gameEnding();
    await_while(gameEndingUpdate());
    initButtonStateInput();
    gameMain();
    await(gameUpdate());
    async_end;
}

static struct async mainLoopAsyncState;
static void mainLoop() {
    asyncMainLoop(&mainLoopAsyncState);
}

int main() {
    //int result = someCppFunction(1,2);
    //printf("result from cpp %i\n", result);
    async_init(&mainLoopAsyncState);
    setMainLoopCallback(&mainLoop);
    return 0;
}
