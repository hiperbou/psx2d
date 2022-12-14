#ifndef ACTORS_H
#define ACTORS_H

#include "actor.h"

Actor* newBee(int file, fix32 x, fix32 y);
Actor* newMotobug(int file, fix32 x, fix32 y);
//Actor* newDummy(const fix32 x, const fix32 y, u16 anim);
//Actor* newShot(const fix32 x, const fix32 y, u16 mode);
Actor* newBlock(int file, int graph, fix32 x, fix32 y, int type);
Actor* newGoal(int file, fix32 x, fix32 y, Actor* target);
Actor* newGoalActivated(int file, fix32 x, fix32 y, Actor * target);

Actor* newCamera(Actor* cameraTarget, fix32 x, fix32 y);


#endif
