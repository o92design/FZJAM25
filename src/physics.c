#include "physics.h"
#include "game.h"
#include <stddef.h>

void HandlePhysics(Game* game, float dt)
{
    (void)game; // currently unused
    for (int entityId = 0; entityId < entityCount; ++entityId)
    {
        Entity* entity = &entities[entityId];
        if (!(entity->tags & TAG_PHYSICS))
        {
            continue;
        }

        // Apply gravity
        entity->velocity.y += GRAVITY * dt;

        // Integrate
        Move(entity, entity->velocity.x, entity->velocity.y, dt);
        
        // Simple ground collision
        entity->onGround = false;
        Entity* ground = NULL;
        for (int otherId = 0; otherId < entityCount; ++otherId)
        {
            Entity* other = &entities[otherId];
            if (other->tags & TAG_PLATFORM)
            {
                ground = other;
                if (CheckCollisionRecs(entity->bounds, ground->bounds))
                {
                    // Snap to ground
                    entity->bounds.y = ground->bounds.y - entity->bounds.height;
                    entity->velocity.y = 0;
                    entity->onGround = true;
                }
            }
        }
        
    }
}

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