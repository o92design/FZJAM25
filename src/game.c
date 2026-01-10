#include "game.h"
#include "input.h"
#include "physics.h"
#include "render.h"

void Game_Init(Game* game)
{
    if (!game) return;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "FZJAM25 - 2D Platformer");
    SetTargetFPS(TARGET_FPS);

    game->state.flippedWorld = false;

    game->ground = Entity_Create("Ground", TAG_PLATFORM, (Vector2){0, 460}, (Rectangle){0, 460, 960, 80}, (Color){70, 65, 90, 255});
    game->player = Entity_Create("Player", TAG_PLAYER | TAG_PHYSICS | TAG_SOLID,
                                 (Vector2){100, (game->ground ? game->ground->bounds.y : 460) - 60},
                                 (Rectangle){100, (game->ground ? game->ground->bounds.y : 460) - 60, 40, 60},
                                 (Color){220, 220, 255, 255});

    if (game->player) {
        game->player->onGround = true;
    }
}

void Game_Loop(Game* game)
{
    if (!game) return;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        if (dt > 0.05f) dt = 0.05f; // Clamp large spikes

        HandleInput(game, dt);
        HandlePhysics(game, dt);

        Camera2D cam = { 0 };
        if (game->player) {
            cam.target = (Vector2){ game->player->bounds.x + game->player->bounds.width * 0.5f, SCREEN_HEIGHT * 0.5f };
        }
        cam.offset = (Vector2){ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
        cam.rotation = 0.0f;
        cam.zoom = 1.0f;

        Render(game, cam);
    }
}

void Game_Shutdown(Game* game)
{
    (void)game; // currently unused
    Entity_Clear();
    CloseWindow();
}
