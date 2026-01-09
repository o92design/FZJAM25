#define _ALLOW_KEYWORD_MACROS

#include "raylib.h"
#include <stdio.h>
#include "entity.h"
#include "config.h"
#include "input.h"
#include "physics.h"

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "FZJAM25 - 2D Platformer");
    SetTargetFPS(60);
    Entity* ground = Entity_Create("Ground", TAG_PLATFORM, (Vector2){0,0}, (Rectangle){0,460,960,80});
    Entity* playerEntity = Entity_Create("Player", TAG_PLAYER | TAG_PHYSICS | TAG_SOLID, (Vector2){100, 100}, (Rectangle){0, ground->position.y - 60,40,60});

    playerEntity->onGround = true;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Input
        float move = GetHorizontalInput();
        ChangeVelocityX(playerEntity, move * PLAYER_SPEED);

        // Jump
        if (GetJumpInput() && playerEntity->onGround)
        {
            ChangeVelocityY(playerEntity, JUMP_VELOCITY);
            playerEntity->onGround = false;
        }

        // Gravity
        //playerEntity->velocity.y += GRAVITY * dt;
        ChangeVelocityY(playerEntity, playerEntity->velocity.y + GRAVITY * dt);

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

        // Render
        BeginDrawing();
        ClearBackground((Color){ 28, 27, 34, 255 });

        BeginMode2D(cam);
        DrawRectangleRec(ground->bounds, (Color){ 70, 65, 90, 255 });
        DrawRectangleV((Vector2){ playerEntity->bounds.x, playerEntity->bounds.y }, (Vector2){ playerEntity->bounds.width, playerEntity->bounds.height }, (Color){ 220, 220, 255, 255 });
        EndMode2D();

        // Debug gamepad info
        char statusText[256];
        bool hasGamepad = false;
        for (int i = 0; i < 4; i++)
        {
            if (IsGamepadAvailable(i))
            {
                hasGamepad = true;
                snprintf(statusText, sizeof(statusText), "Gamepad %d: %s | Move: A/D or Left Stick | Jump: Space/W or A button", 
                         i, GetGamepadName(i));
                break;
            }
        }
        
        if (!hasGamepad)
        {
            snprintf(statusText, sizeof(statusText), "No gamepad detected | Move: A/D | Jump: Space/W (Press any gamepad button to connect)");
        }
        
        DrawText(statusText, 10, 10, 18, RAYWHITE);

        EndDrawing();
    }

    Entity_Clear();
    CloseWindow();

    return 0;
}
