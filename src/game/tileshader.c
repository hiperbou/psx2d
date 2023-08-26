#include <stdbool.h>
#include "tileshader.h"
#include "../core/hgl_actor.h"
#include "../core/hgl_mem.h"

#include "../engine/tilemap.h"
#include "../core/hgl_anim.h"
#include "enemyupdate.h" //setAnimation

static TileMap* tileMap;


typedef struct {
    uint8_t *tilePtr;
    uint8_t tileId;
    int * currentFramePtr;
} AnimatedTile;

static AnimatedTile *animatedTiles;
static int numAnimatedTiles = 0;


static void update(Actor* actor) {
    int tilesToAnimate = numAnimatedTiles;
    AnimatedTile * animatedTilePtr = animatedTiles;

    while(tilesToAnimate>0) {
        if (*(animatedTilePtr->tilePtr) <= 1) { //Remove deleted tiles TODO: don't depend on tile id
            AnimatedTile* lastAnimatedTilePtr = animatedTiles + (numAnimatedTiles-1);
            (*animatedTilePtr) = *lastAnimatedTilePtr; //Replace current animated tile by the last one
            numAnimatedTiles--;
        } else {
            *(animatedTilePtr->tilePtr) = *(animatedTilePtr->currentFramePtr);
            animatedTilePtr++;
        }
        tilesToAnimate--;
    }
}

#define TILE_ANIMATOR(ANIMATIONSTATE, ANIM, DELAY)\
static AnimationState* ANIMATIONSTATE;\
ANIMATIONSTATE = HGL_ANIM_new();\
SetAnimationState(ANIMATIONSTATE, ANIM, DELAY);\

#define ANIMATED_TILE(TILE, ANIMATIONSTATE) \
        case TILE: \
        animatedTilePtr->tilePtr = tilePtr;\
        animatedTilePtr->tileId = *tilePtr;\
        animatedTilePtr->currentFramePtr = &ANIMATIONSTATE->currentFrame;\
        animatedTilePtr++;\
        numAnimatedTiles++;\
        break;

#define TILE_QUESTIONBOX 10
#define TILE_BRICK 77


static void constructor(Actor* actor) {

    ANIM(Brick, 92,93,94,95)
    ANIM(Pipe, 8,11,9)

    TILE_ANIMATOR(BrickAnimator, Brick, 6)
    TILE_ANIMATOR(PipeAnimator, Pipe, 10)
    
    int numCols = tileMap->numCols;
    int numRows = tileMap->numRows;
    int numTiles = numCols * numRows;

    animatedTiles = HGL_malloc(numTiles * sizeof(AnimatedTile)); //Optimize this
    numAnimatedTiles = 0;

    uint8_t *tilePtr = (uint8_t*)tileMap->map;
    AnimatedTile * animatedTilePtr = animatedTiles;


    while(numTiles>0) {
        switch (*tilePtr)
        {
        case 0:
            break;
        ANIMATED_TILE(TILE_QUESTIONBOX, PipeAnimator)
        ANIMATED_TILE(TILE_BRICK,       BrickAnimator)


        default:
            break;
        }
        tilePtr++;
        numTiles--;
    }
}

Actor* newTileShader(TileMap* _tileMap) {
    tileMap = _tileMap;
    return newActor(0, 0, 0, 0, constructor, update);
}
