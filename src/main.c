#include "raylib.h"
#include <stdio.h>

// Basic constants for tuning feel
static const int screenWidth = 960;
static const int screenHeight = 540;
static const float playerSpeed = 260.0f;
static const float gravity = 900.0f;
static const float jumpVelocity = -420.0f;
static const Rectangle groundRect = { 0, 460, 960, 80 };

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

    Vector2 playerPos = { 100, groundRect.y - 48 }; // start on ground
    Vector2 velocity = { 0 }; // player velocity
    Rectangle playerRect = { playerPos.x, playerPos.y, 32, 48 };

    bool onGround = true;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Input
        float move = GetHorizontalInput();
        velocity.x = move * playerSpeed;

        // Jump
        if (GetJumpInput() && onGround)
        {
            velocity.y = jumpVelocity;
            onGround = false;
        }

        // Gravity
        velocity.y += gravity * dt;

        // Integrate
        playerPos.x += velocity.x * dt;
        playerPos.y += velocity.y * dt;

        // Simple ground collision
        playerRect.x = playerPos.x;
        playerRect.y = playerPos.y;

        onGround = false;
        if (CheckCollisionRecs(playerRect, groundRect))
        {
            // Snap to ground
            playerPos.y = groundRect.y - playerRect.height;
            velocity.y = 0;
            onGround = true;
        }

        // Camera follow
        Vector2 cameraTarget = { playerPos.x + playerRect.width * 0.5f, screenHeight * 0.5f };
        Camera2D cam = { 0 };
        cam.target = cameraTarget;
        cam.offset = (Vector2){ screenWidth * 0.5f, screenHeight * 0.5f };
        cam.rotation = 0.0f;
        cam.zoom = 1.0f;

        // Render
        BeginDrawing();
        ClearBackground((Color){ 28, 27, 34, 255 });

        BeginMode2D(cam);
        DrawRectangleRec(groundRect, (Color){ 70, 65, 90, 255 });
        DrawRectangleV((Vector2){ playerPos.x, playerPos.y }, (Vector2){ playerRect.width, playerRect.height }, (Color){ 220, 220, 255, 255 });
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
