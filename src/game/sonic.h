#ifndef DEMO_SONIC_H
#define DEMO_SONIC_H

#include <stdbool.h>
#include "actor.h"


typedef struct PlayerEventHandler PlayerEventHandler;

typedef struct PlayerEventHandler{
    TileMap *tilemap;
    TileMap *collisionTilemap;
    Actor * player;
    void (*onColidedWithCeilingTile)(PlayerEventHandler*, Tile);
    bool (*onColidedWithFloorTile)(PlayerEventHandler*, Tile);
}PlayerEventHandler;

Actor* newSonic(int file, fix32 x, fix32 y, TileMap _collisionTilemap, PlayerEventHandler* playerEventHandler);


#endif //DEMO_SONIC_H
