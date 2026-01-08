#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"

typedef struct {
    int id;
    bool active;
    int tags;
    char name[50];
    Vector2 velocity;
    Rectangle bounds;
    bool onGround;
} Entity;

#endif // ENTITY_H