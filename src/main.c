#include "core/hgl.h"
#include "game.h"
#include "gameresources.h"
//#include "cppfunction.h"

int gameMain();
int gameUpdate();

int gameTitle();
int gameTitleUpdate();

int gameEnding();
int gameEndingUpdate();

void mainLoop() {
    static int mainLoopState = 0;

    switch(mainLoopState) {
        case 0:
            gameInit();
            initResources();
            gameTitle();
            mainLoopState++;
        case 1:
            if(!gameTitleUpdate()) {
                mainLoopState++;
            }
        break;
        case 2:
            gameEnding();
            mainLoopState++;
        case 3:
            if(!gameEndingUpdate()) {
                mainLoopState++;
            }
        break;
        case 4:
            initButtonStateInput();
            gameMain();
            mainLoopState++;
        case 5:
            gameUpdate();
        break;
    }
}

int main() {
    //int result = someCppFunction(1,2);
    //printf("result from cpp %i\n", result);
    setMainLoopCallback(&mainLoop);
    return 0;
}
