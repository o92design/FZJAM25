#define _ALLOW_KEYWORD_MACROS

#include "raylib.h"
#include <stdio.h>
#include "entity.h"

#define TAG_PLATFORM 1
#define TAG_PLAYER   2
#define TAG_ENEMY    4

// Basic constants for tuning feel
static const int screenWidth = 960;
static const int screenHeight = 540;
static const float playerSpeed = 260.0f;
static const float gravity = 900.0f;
static const float jumpVelocity = -420.0f;
static const Entity ground = { -1, true, TAG_PLATFORM, "Ground", {0,0}, {0,460,960,80} };

static float GetHorizontalInput(void)
{
    float axis = 0.0f;

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) axis += 1.0f;
    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) axis -= 1.0f;

    // Check all possible gamepads (web sometimes uses non-zero indices)
    for (int i = 0; i < 4; i++)
    {
        if (IsGamepadAvailable(i))
        {
            float gpAxis = GetGamepadAxisMovement(i, GAMEPAD_AXIS_LEFT_X);
            if (gpAxis < -0.2f || gpAxis > 0.2f) 
            {
                axis = gpAxis;
                break;
            }
        }
    }

    return axis;
}

static bool GetJumpInput(void)
{
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        return true;
    
    // Check all gamepads for button press (A button on Xbox = button 0)
    for (int i = 0; i < 4; i++)
    {
        if (IsGamepadAvailable(i))
        {
            if (IsGamepadButtonPressed(i, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) ||  // A button
                IsGamepadButtonPressed(i, 0))  // Also check button 0 directly
            {
                return true;
            }
        }
    }
    
    return false;
}

int main(void)
{
    InitWindow(screenWidth, screenHeight, "FZJAM25 - 2D Platformer");
    SetTargetFPS(60);

    Entity playerEntity = { 0, true, TAG_PLAYER, "Player", {0,0}, {100, ground.bounds.y - 48,32,48} };

    playerEntity.onGround = true;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Input
        float move = GetHorizontalInput();
        playerEntity.velocity.x = move * playerSpeed;

        // Jump
        if (GetJumpInput() && playerEntity.onGround)
        {
            playerEntity.velocity.y = jumpVelocity;
            playerEntity.onGround = false;
        }

        // Gravity
        playerEntity.velocity.y += gravity * dt;

        // Integrate
        playerEntity.bounds.x += playerEntity.velocity.x * dt;
        playerEntity.bounds.y += playerEntity.velocity.y * dt;
        // Simple ground collision
        playerEntity.onGround = false;
        if (CheckCollisionRecs(playerEntity.bounds, ground.bounds))
        {
            // Snap to ground
            playerEntity.bounds.y = ground.bounds.y - playerEntity.bounds.height;
            playerEntity.velocity.y = 0;
            playerEntity.onGround = true;
        }

        // Camera follow
        Vector2 cameraTarget = { playerEntity.bounds.x + playerEntity.bounds.width * 0.5f, screenHeight * 0.5f };
        Camera2D cam = { 0 };
        cam.target = cameraTarget;
        cam.offset = (Vector2){ screenWidth * 0.5f, screenHeight * 0.5f };
        cam.rotation = 0.0f;
        cam.zoom = 1.0f;

        // Render
        BeginDrawing();
        ClearBackground((Color){ 28, 27, 34, 255 });

        BeginMode2D(cam);
        DrawRectangleRec(ground.bounds, (Color){ 70, 65, 90, 255 });
        DrawRectangleV((Vector2){ playerEntity.bounds.x, playerEntity.bounds.y }, (Vector2){ playerEntity.bounds.width, playerEntity.bounds.height }, (Color){ 220, 220, 255, 255 });
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

    CloseWindow();
    return 0;
}
