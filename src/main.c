#define _ALLOW_KEYWORD_MACROS

#include "raylib.h"
#include <stdio.h>
#include "entity.h"
#include "config.h"
#include "input.h"
#include "physics.h"
#include "render.h"

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "FZJAM25 - 2D Platformer");
    SetTargetFPS(60);
    Entity* ground = Entity_Create("Ground", TAG_PLATFORM, (Vector2){0,0}, (Rectangle){0,460,960,80}, (Color){70,65,90,255});
    Entity* playerEntity = Entity_Create("Player", TAG_PLAYER | TAG_PHYSICS | TAG_SOLID, (Vector2){100, 100}, (Rectangle){0, ground->position.y - 60,40,60}, (Color){220,220,255,255});

    playerEntity->onGround = true;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        HandleInput(dt);

        // Integrate
        Move(playerEntity, playerEntity->velocity.x, playerEntity->velocity.y, dt);
        
        // Simple ground collision
        playerEntity->onGround = false;
        if (CheckCollisionRecs(playerEntity->bounds, ground->bounds))
        {
            // Snap to ground
            playerEntity->bounds.y = ground->bounds.y - playerEntity->bounds.height;
            playerEntity->velocity.y = 0;
            playerEntity->onGround = true;
        }

        // Camera follow
        Vector2 cameraTarget = { playerEntity->bounds.x + playerEntity->bounds.width * 0.5f, SCREEN_HEIGHT * 0.5f };
        Camera2D cam = { 0 };
        cam.target = cameraTarget;
        cam.offset = (Vector2){ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
        cam.rotation = 0.0f;
        cam.zoom = 1.0f;

        Render(cam);
    }

    Entity_Clear();
    CloseWindow();

    return 0;
}
