#include <stdbool.h>
#include "tileshader.h"
#include "actor.h"
#include "hgl_mem.h"

#include "sprites.h" //TileMap
#include "hgl_anim.h"
#include "enemyupdate.h" //setAnimation


#define TILE_QUESTIONBOX 10
#define TILE_BRICK 77

ANIM(Brick, 92,93,94,95)
ANIM(Pipe, 8,11,9)

static AnimationState* animationState;
static AnimationState* animationState2;

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
        if (*(animatedTilePtr->tilePtr) <= 1) { //Remove deleted tiles
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

static void constructor(Actor* actor) {

    animationState = HGL_ANIM_new();
    SetAnimationState(animationState, Brick, 6);

    animationState2 = HGL_ANIM_new();
    SetAnimationState(animationState2, Pipe, 10);
    

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
        
        case TILE_QUESTIONBOX:
            animatedTilePtr->tilePtr = tilePtr;
            animatedTilePtr->tileId = *tilePtr;
            animatedTilePtr->currentFramePtr = &animationState2->currentFrame;
            animatedTilePtr++;
            numAnimatedTiles++;
            break;
        case TILE_BRICK:
            animatedTilePtr->tilePtr = tilePtr;
            animatedTilePtr->tileId = *tilePtr;
            animatedTilePtr->currentFramePtr = &animationState->currentFrame;
            animatedTilePtr++;
            numAnimatedTiles++;
        break;

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
