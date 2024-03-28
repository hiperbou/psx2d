#include "core/hgl.h"
#include "game.h"
#include "gameresources.h"

#include "utils/async.h"
#include "utils/script.h"
//#include "cppfunction.h"

int gameMain();
int gameUpdate();

int gameTitle();
int gameTitleUpdate();

int gameEnding();
int gameEndingUpdate();

static void mainLoop() {
    script_begin
        gameInit();
        initResources();
        gameTitle();
        script_await_while(gameTitleUpdate());
        gameEnding();
        script_await_while(gameEndingUpdate());
        initButtonStateInput();
        gameMain();
        script_await(gameUpdate());
    script_end;
}

int main() {
    //int result = someCppFunction(1,2);
    //printf("result from cpp %i\n", result);
    setMainLoopCallback(&mainLoop);
    return 0;
}
