#include "input.h"


void HandleInput(float dt) {
    // Input
    float move = GetHorizontalInput();
    bool jumpInput = GetJumpInput();

    for(int entityId = 0; entityId < entityCount; ++entityId) {
        Entity* playerEntity = &entities[entityId];
        if(!(playerEntity->tags & TAG_PLAYER)) {
            continue;
        }

        ChangeVelocityX(playerEntity, move * PLAYER_SPEED);

        // Jump
        if (GetJumpInput() && playerEntity->onGround)
        {
            ChangeVelocityY(playerEntity, JUMP_VELOCITY);
            playerEntity->onGround = false;
        }

        // Gravity
        ChangeVelocityY(playerEntity, playerEntity->velocity.y + GRAVITY * dt);
    }
        
}

bool GetJumpInput(void)
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

float GetHorizontalInput(void)
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

void ChangeVelocity(Entity* entity, float velocityX, float velocityY)
{
    if (entity)
    {
        entity->velocity.x = velocityX;
        entity->velocity.y = velocityY;
    }
}

void ChangeVelocityY(Entity* entity, float velocityY)
{
    ChangeVelocity(entity, entity->velocity.x, velocityY);
}

void ChangeVelocityX(Entity* entity, float velocityX)
{
    ChangeVelocity(entity, velocityX, entity->velocity.y);
}