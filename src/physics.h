#include "entity.h"
#include "config.h"

void HandlePhysics(float dt);

void Move(Entity* entity, float velocityX, float velocityY, float dt);
void MoveX(Entity* entity, float velocityX, float dt);
void MoveY(Entity* entity, float velocityY, float dt);