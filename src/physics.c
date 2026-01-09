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
    Move(entity, velocityX, 0.0f, dt);
}

void MoveY(Entity* entity, float velocityY, float dt)
{
    Move(entity, 0.0f, velocityY, dt);
}