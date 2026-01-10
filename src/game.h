#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "entity.h"
#include "config.h"

typedef struct {
    bool flippedWorld;
} GameState;

typedef struct {
    GameState state;
    Entity* player;
    Entity* ground;
} Game;

void Game_Init(Game* game);
void Game_Loop(Game* game);
void Game_Shutdown(Game* game);

#endif // GAME_H
