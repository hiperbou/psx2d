#ifndef DEMO_SONIC_H
#define DEMO_SONIC_H

#include <stdbool.h>
#include "../core/hgl_actor.h"
#include "../engine/tilemap.h"

typedef struct PlayerEventHandler PlayerEventHandler;

typedef struct PlayerEventHandler{
    TileMap *tilemap;
    TileMap *collisionTileMap;
    Actor * player;
    void (*onColidedWithCeilingTile)(PlayerEventHandler*, Tile);
    bool (*onColidedWithFloorTile)(PlayerEventHandler*, Tile);
    bool (*onGrounded)(PlayerEventHandler*, Tile);
}PlayerEventHandler;

Actor* newSonic(int file, fix32 x, fix32 y, TileMap _collisionTileMap, PlayerEventHandler* playerEventHandler);


#endif //DEMO_SONIC_H
