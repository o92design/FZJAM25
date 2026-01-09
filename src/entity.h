#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"

typedef struct {
    int id;
    bool active;
    int tags;           // Bitflags: TAG_PLAYER, TAG_PLATFORM, etc
    char name[50];
    Vector2 position;   // World position
    Rectangle bounds;   // Position + size for collision
    Vector2 velocity;
    bool onGround;      // Useful for platformers
    Color boundColor;
} Entity;

// Entity array management
extern Entity entities[256];
extern int entityCount;

Entity* Entity_Create(const char* name, int tags, Vector2 pos, Rectangle bounds);
void Entity_Destroy(int id);
Entity* Entity_GetById(int id);
void Entity_Clear(void);

#endif // ENTITY_H