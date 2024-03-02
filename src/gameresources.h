#ifndef GAMERESOURCES_H
#define GAMERESOURCES_H

typedef struct GameResources {
    int (*getFontAtlas)();
    int (*getSonicFpg)();
    int (*getEnemiesFpg)();
    int (*getBlocksFpg)();
} GameResources;

extern GameResources Resources;

void initResources();




#endif //GAMERESOURCES_H
