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

void initResources() {
    Resources.getFontAtlas = loadFontAtlas;
    Resources.getSonicFpg = loadSonicFpg;
    Resources.getEnemiesFpg = loadEnemiesFpg;
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