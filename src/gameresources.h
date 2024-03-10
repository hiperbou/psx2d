#ifndef GAMERESOURCES_H
#define GAMERESOURCES_H

typedef struct GameResources {
    int (*getFontAtlas)();
    int (*getSonicFpg)();
    int (*getEnemiesFpg)();

    int (*getSmb3tTileset)();
    int (*getSmb3t2Tileset)();
    int (*getSmb3t3Tileset)();
    int (*getSmb3t4Tileset)();

    int (*getSmb3uTileset)();
    int (*getSmb3hTileset)();

    int (*getIntroTileset)();
} GameResources;

extern GameResources Resources;

void initResources();




#endif //GAMERESOURCES_H
