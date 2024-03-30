#include "core/hgl.h"
#include "game.h"
#include "gameresources.h"

#include "utils/async.h"
#include "utils/script.h"
//#include "cppfunction.h"

void initGameState();
int gameMain();
int gameUpdate();

int gameTitle();
int gameTitleUpdate();

int gameEnding();
int gameEndingUpdate();

static void mainLoop() {
    script_begin
        initGameState();
        gameTitle();
        script_await_while(gameTitleUpdate());
        initButtonStateInput();
        gameMain();
        script_await_while(gameUpdate());
        gameEnding();
        script_await_while(gameEndingUpdate());
    script_end;
}

int main() {
    //int result = someCppFunction(1,2);
    //printf("result from cpp %i\n", result);
    gameInit();
    initResources();

    setMainLoopCallback(&mainLoop);
    return 0;
}
