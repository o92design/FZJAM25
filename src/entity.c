#ifndef ENTITY_C
#define ENTITY_C
#include "entity.h"
#include "config.h"
#include <string.h>

// Global entity storage defined here (declared extern in entity.h)
Entity entities[MAX_ENTITIES] = { 0 };
int entityCount = 0;
static int nextEntityId = 0; // Stable, monotonically increasing IDs

Entity* Entity_Create(const char* name, int tags, Vector2 pos, Rectangle bounds)
{
    if (entityCount >= MAX_ENTITIES) return NULL;

    Entity* ent = &entities[entityCount++];
    ent->id = nextEntityId++;
    ent->active = true;
    ent->tags = tags;
    strncpy(ent->name, name, sizeof(ent->name) - 1);
    ent->name[sizeof(ent->name) - 1] = '\0';
    ent->position = pos;
    ent->bounds = bounds;
    ent->velocity = (Vector2){0, 0};
    ent->onGround = false;

    return ent;
}

void Entity_Destroy(int id)
{
    for (int i = 0; i < entityCount; i++)
    {
        if (entities[i].id == id)
        {
            entities[i] = entities[entityCount - 1]; // Replace with last entity for compact array
            entityCount--;
            // Keep the moved entity's ID stable; just clear the old last slot
            memset(&entities[entityCount], 0, sizeof(Entity));
            return;
        }
    }
}

Entity* Entity_GetById(int id)
{
    for (int i = 0; i < entityCount; i++)
    {
        if (entities[i].id == id)
        {
            return &entities[i];
        }
    }
    return NULL;
}

void Entity_Clear(void)
{
    // Zero-out all entities for a clean slate
    memset(entities, 0, sizeof(entities));
    entityCount = 0;
    nextEntityId = 0;
}

#endif // ENTITY_C