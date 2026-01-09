# TODO - FZJAM25 Entity-System Architecture

## **Concept: Data-Oriented Entity System**

- **Entities** = Pure data (position, velocity, bounds, tags)
- **Systems** = Functions that operate on entity arrays
- **Game loop** = Call systems in order (Input → Physics → Render)

---

## **Phase 1: Core Entity & Systems**

- [x] **entity.h / entity.c** - Entity management
  - Keep existing `Entity` struct, maybe add:
    - `bool active;` - skip inactive entities
    - `int tags;` - bitflags (PLAYER, ENEMY, PLATFORM, etc.)
  - `Entity* CreateEntity(const char* name, Vector2 pos, Rectangle bounds)`
  - `Entity entities[MAX_ENTITIES];` - global array or game state struct
  - `int entityCount;`

- [ ] **systems.h / systems.c** - All game systems
  - `InputSystem(Entity* entities, int count, float dt)` - reads input, updates player entity velocity based on keys/gamepad
  - `PhysicsSystem(Entity* entities, int count, float dt)` - applies gravity, velocity, collision detection
  - `RenderSystem(Entity* entities, int count, Camera2D cam)` - draws all visible entities
  - Each system loops over entities and processes relevant ones (check tags)

- [ ] **config.h** - Constants & tuning values
  - `#define GRAVITY 900.0f`
  - `#define PLAYER_SPEED 260.0f`
  - `#define JUMP_VELOCITY -420.0f`
  - `#define MAX_ENTITIES 256`
  - Easy to tweak gameplay feel

## **Phase 2: Input & Camera**

- [ ] **input.h / input.c** - Raw input polling (no entity logic)
  - `float GetHorizontalInput()` - return -1.0 to 1.0
  - `bool GetJumpPressed()`
  - `char* GetGamepadDebugString()` - for UI
  - Systems call these, not main loop

- [ ] **camera.h / camera.c** - Camera utilities
  - `UpdateCamera(Camera2D* cam, Entity* target, int screenW, int screenH)`
  - Can be called from main or as separate CameraSystem()

## **Phase 3: Game State & Level Data**

- [ ] **game.h / game.c** - Game state container
  
  - ```c
    typedef struct Game {
        Entity entities[MAX_ENTITIES];
        int entityCount;
        Camera2D camera;
        GameState state; // MENU, PLAYING, PAUSED
    } Game;
  
  - ```c
  
    `Game_Init()`, `Game_Update(float dt)`, `Game_Render()`
    `Game_Update()` calls all systems in sequence

- [ ] **level.h / level.c** - Level loader
  
  - `LoadLevel(Game* game, int levelIndex)` - spawns platform entities
  - Could use simple arrays or later load from files
  - Creates Entity for each platform with PLATFORM tag

## **Phase 4: Rendering & Polish**

- [ ] **render.h / render.c** - Extended rendering
  - `DrawEntity(Entity* e, Camera2D cam)` - smart draw based on tags
  - `DrawUI(Game* game)` - HUD, FPS, debug info
  - Keep RenderSystem() simple, delegate details here

- [ ] **assets.h / assets.c** - Asset management (optional for jam)
  - Load textures, sounds
  - `Texture2D GetEntityTexture(int tags)` - return sprite based on entity type

---

## **Implementation Order:**

1. **Entity arrays** - replace single `playerEntity` with `Entity entities[MAX_ENTITIES]`
2. **InputSystem** - extract input logic to system function
3. **PhysicsSystem** - extract physics to system function
4. **RenderSystem** - extract rendering to system function
5. **config.h** - move constants out of main.c
6. **Game struct** - wrap everything in Game state
7. **Level loading** - spawn multiple platform entities

## **File Structure After Refactor:**

```c
src/
  main.c          # Init raylib, call Game_Update/Game_Render, cleanup
  entity.h/c      # Entity struct, creation, array management
  systems.h/c     # InputSystem, PhysicsSystem, RenderSystem, etc.
  config.h        # Constants (GRAVITY, SPEED, MAX_ENTITIES)
  game.h/c        # Game struct, game state machine
  input.h/c       # Raw input functions (GetHorizontalInput, etc.)
  camera.h/c      # Camera utilities
  level.h/c       # Level loading/generation
  render.h/c      # Rendering utilities (optional split)
  assets.h/c      # Asset loading (optional for jam)
```

## **Example System Pattern:**

```c
void PhysicsSystem(Entity* entities, int count, float dt) {
    for (int i = 0; i < count; i++) {
        Entity* e = &entities[i];
        if (!e->active) continue;
        
        // Only apply physics to entities with PHYSICS tag
        if (e->tags & TAG_PHYSICS) {
            e->velocity.y += GRAVITY * dt;
            e->position.x += e->velocity.x * dt;
            e->position.y += e->velocity.y * dt;
            e->bounds.x = e->position.x;
            e->bounds.y = e->position.y;
        }
    }
}
```

## **Notes**

- Systems are pure functions - easy to test, debug, disable
- Entity data is centralized - cache-friendly iteration
- Adding new entity types = just add to array with different tags
- No inheritance/vtables - fast, simple C code
- Scale from 1 player entity to 100+ enemies easily

- Keep header files minimal (declarations only)
- Use consistent naming: `ModuleName_FunctionName()`
- Test after each phase - don't break working code
- Phase 4 is optional depending on game jam time constraints
