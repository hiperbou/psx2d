#include "gameresources.h"
#include "media/fpg.h"

GameResources Resources;

#define system_fpg 0

#define RESOURCE(NAME) \
static int NAME##_res; \
int get##NAME() { return NAME##_res; } \
int load##NAME() { \
    NAME##_res =

#define RESOURCE_END(NAME) \
    Resources.get##NAME = get##NAME; \
    return NAME##_res; \
}

RESOURCE(FontAtlas)
load_atlas(system_fpg, "art/gbs-mono", 8,8,16,14);
RESOURCE_END(FontAtlas)

RESOURCE(SonicFpg)
new_fpg();
load_atlas(SonicFpg_res, "art/sonic", 48, 48, 5, 5);
RESOURCE_END(SonicFpg)

RESOURCE(EnemiesFpg)
new_fpg();
load_atlas(EnemiesFpg_res, "art/enemies", 48, 32, 4, 2);
RESOURCE_END(EnemiesFpg)

RESOURCE(Smb3tTileset)
new_fpg();
load_atlas(Smb3tTileset_res, "art/smb3t" , 16, 16, 11, 9);
RESOURCE_END(Smb3tTileset)

RESOURCE(Smb3t2Tileset)
new_fpg();
load_atlas(Smb3t2Tileset_res, "art/smb3t2" , 16, 16, 11, 9);
RESOURCE_END(Smb3t2Tileset)

RESOURCE(Smb3t3Tileset)
new_fpg();
load_atlas(Smb3t3Tileset_res, "art/smb3t3" , 16, 16, 11, 9);
RESOURCE_END(Smb3t3Tileset)

RESOURCE(Smb3t4Tileset)
new_fpg();
load_atlas(Smb3t4Tileset_res, "art/smb3t4" , 16, 16, 11, 9);
RESOURCE_END(Smb3t4Tileset)

RESOURCE(Smb3uTileset)
new_fpg();
load_atlas(Smb3uTileset_res, "art/smb3u" , 16, 16, 4, 2);
RESOURCE_END(Smb3uTileset)

RESOURCE(Smb3hTileset)
new_fpg();
load_atlas(Smb3hTileset_res, "art/smb3h" , 16, 16, 4, 3);
RESOURCE_END(Smb3hTileset)

RESOURCE(IntroTileset)
new_fpg();
load_atlas(IntroTileset_res, "art/intro" , 16, 16, 10, 9);
RESOURCE_END(IntroTileset)

void initResources() {
    Resources.getFontAtlas = loadFontAtlas;
    Resources.getSonicFpg = loadSonicFpg;
    Resources.getEnemiesFpg = loadEnemiesFpg;
    Resources.getSmb3tTileset = loadSmb3tTileset;
    Resources.getSmb3t2Tileset = loadSmb3t2Tileset;
    Resources.getSmb3t3Tileset = loadSmb3t3Tileset;
    Resources.getSmb3t4Tileset = loadSmb3t4Tileset;

    Resources.getSmb3uTileset = loadSmb3uTileset;
    Resources.getSmb3hTileset = loadSmb3hTileset;

    Resources.getIntroTileset = loadIntroTileset;
}

#undef system_fpg

/*
static int font_atlas;
int getFontAtlas() { return font_atlas; }
int loadFontAtlas() {
    font_atlas = load_atlas(system_fpg, "art/gbs-mono", 8,8,16,14);
    Resources.getFontAtlas = getFontAtlas;
    return font_atlas;
}

static int sonic_fpg;
int getSonicFpg() { return sonic_fpg; }
int loadSonicFpg() {
    sonic_fpg = new_fpg();
    load_atlas(sonic_fpg, "art/sonic", 48, 48, 5, 5);
    Resources.getSonicFpg = getSonicFpg;
    return sonic_fpg;
}
static int enemies_fpg;
int getEnemiesFpg() { return enemies_fpg; }
int loadEnemiesFpg() {
    enemies_fpg = new_fpg();
    load_atlas(enemies_fpg, "art/enemies", 48, 32, 4, 2);
    Resources.getEnemiesFpg = getEnemiesFpg;
    return enemies_fpg;
}*/