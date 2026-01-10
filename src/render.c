#include "render.h"
#include "game.h"
#include <stdio.h>

void Render(Game* game, Camera2D camera)
{
    (void)game; // currently unused
    BeginDrawing();
    ClearBackground((Color){ 28, 27, 34, 255 });

    BeginMode2D(camera);
    for (int i = 0; i < entityCount; i++)
    {
        Entity* entity = &entities[i];
        if (entity->active)
        {
            DrawRectangle(entity->bounds.x, entity->bounds.y, entity->bounds.width, entity->bounds.height, entity->boundColor);
        }
    }
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