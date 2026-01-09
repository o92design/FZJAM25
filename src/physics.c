#include "physics.h"

void Move(Entity* entity, float velocityX, float velocityY, float dt)
{
    if (entity)
    {
        entity->bounds.x += velocityX * dt;
        entity->bounds.y += velocityY * dt;
    }
}

void MoveX(Entity* entity, float velocityX, float dt)
{
    Move(entity, velocityX, entity->velocity.y, dt);
}

void MoveY(Entity* entity, float velocityY, float dt)
{
    Move(entity, entity->velocity.x, velocityY, dt);
}