#ifndef GAMERESOURCES_H
#define GAMERESOURCES_H

typedef struct GameResources {
    int (*getFontAtlas)();
    int (*getSonicFpg)();
    int (*getEnemiesFpg)();
} GameResources;

extern GameResources Resources;

void initResources();




#endif //GAMERESOURCES_H
