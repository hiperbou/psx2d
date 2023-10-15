
#include "actors.h"

bool checkCollision(Actor * actor, Actor * targetActor) {
    if(targetActor->entity->x < actor->entity->x - FIX32(16) ||
       targetActor->entity->x > actor->entity->x + FIX32(16) ||
       targetActor->entity->y < actor->entity->y - FIX32(16) ||
       targetActor->entity->y > actor->entity->y + FIX32(16)) return false;
    return true;
}
