#ifndef CONFIG_H
#define CONFIG_H

// Screen
#define SCREEN_WIDTH  960
#define SCREEN_HEIGHT 540
#define TARGET_FPS    60

// Physics
#define GRAVITY           900.0f
#define PLAYER_SPEED      260.0f
#define JUMP_VELOCITY     -420.0f

// Entity system
#define MAX_ENTITIES 256

// Entity tags (bitflags)
#define TAG_NONE     0
#define TAG_PLATFORM 1
#define TAG_PLAYER   2
#define TAG_ENEMY    4
#define TAG_PHYSICS  8  // Entities affected by gravity
#define TAG_SOLID    16 // Entities with collision

#endif // CONFIG_H
