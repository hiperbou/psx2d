#include "game.h"
#include "gameresources.h"
//#include "cppfunction.h"

int gameMain();
int gameUpdate();

int gameTitle();
int gameTitleUpdate();

int gameEnding();
int gameEndingUpdate();

int main() {
    //int result = someCppFunction(1,2);
    //printf("result from cpp %i\n", result);

    gameInit();
    initResources();

    gameTitle();
    while(gameTitleUpdate()){ }

    gameEnding();
    while(gameEndingUpdate()){ }

    initButtonStateInput();

    gameMain();
    while(1)
    {
        gameUpdate();
    }

    return 0;
}
