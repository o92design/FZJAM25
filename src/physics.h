#ifndef PHYSICS_H
#define PHYSICS_H

#include "entity.h"
#include "config.h"
#include "game.h"

void HandlePhysics(Game* game, float dt);

void Move(Entity* entity, float velocityX, float velocityY, float dt);
void MoveX(Entity* entity, float velocityX, float dt);
void MoveY(Entity* entity, float velocityY, float dt);

#endif // PHYSICS_H