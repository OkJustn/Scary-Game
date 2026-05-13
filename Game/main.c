#include "raylib.h"
// raymath needs this define before the include to work as a single header
#define RAYMATH_STATIC_INLINE 
#include "raymath.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define MAP_WIDTH 12
#define MAP_HEIGHT 12
#define TILE_SIZE 64.0f 
#define FOV (60.0f * DEG2RAD)  // 60 deg fov
#define NUM_RAYS SCREEN_WIDTH 
#define MAX_DEPTH 5.0f    
#define DOOR_TILE 2

typedef struct Player { 
    Vector2 position;
    float angle;
    float speed; 
    float rotSpeed; 
    bool hasKey;
} Player;

typedef struct Monster { 
    Vector2 position; 
    float speed; 
    bool active; 
} Monster;

typedef struct Key { 
    Vector2 position; 
    bool active; 
} Key;

typedef struct RayHit { 
    float distance; 
    bool hitVertical; // vertical wall texturing 
    float wallXHit; 
    float wallYHit; 
    int tileHit; // 0=none, 1=wall, 2=door
} RayHit;

typedef struct GameState {
    int map[MAP_HEIGHT][MAP_WIDTH]; // current level map
    float depthBuffer[SCREEN_WIDTH]; // for sprite rendering
    bool gameOver;  
    float gameOverTimer; // duration for jumspcare
} GameState;

bool isWall(int x, int y, int map[MAP_HEIGHT][MAP_WIDTH], bool hasKey) {
    // checks out of bounds
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return true; 
    
    // if cell is a door and player has not key == wall
    if (map[y][x] == 2 && !hasKey) return true;
    
    // return if wall or not
    return (map[y][x] == 1); 
}

void LoadLevel(int levelNum, Player *p, Monster *m, Key *k, GameState *state, int allLevels[5][MAP_HEIGHT][MAP_WIDTH]) {
    // convert to 0 based index
    int lvl = levelNum - 1;
    
    // assign map for current level to game state
    memcpy(state->map, allLevels[lvl], sizeof(state->map));

    // default states
    p->hasKey = false;
    k->active = true;

    // different spawn points each level
    switch(levelNum) {
        case 1:
            p->position = (Vector2){ TILE_SIZE * 1.5f,  TILE_SIZE * 1.5f  }; // player
            m->position = (Vector2){ TILE_SIZE * 1.5f,  TILE_SIZE * 9.5f  }; // monster
            k->position = (Vector2){ TILE_SIZE * 10.5f, TILE_SIZE * 1.5f  }; // key
            break;
        case 2:
            p->position = (Vector2){ TILE_SIZE * 1.5f,  TILE_SIZE * 10.5f };
            m->position = (Vector2){ TILE_SIZE * 1.5f,  TILE_SIZE * 1.5f  };
            k->position = (Vector2){ TILE_SIZE * 10.5f, TILE_SIZE * 1.5f  };
            break;
        case 3:
            p->position = (Vector2){ TILE_SIZE * 1.5f,  TILE_SIZE * 1.5f  };
            m->position = (Vector2){ TILE_SIZE * 10.5f, TILE_SIZE * 10.5f };
            k->position = (Vector2){ TILE_SIZE * 6.5f,  TILE_SIZE * 5.5f  };
            break;
        case 4:
            p->position = (Vector2){ TILE_SIZE * 5.5f,  TILE_SIZE * 5.5f  };
            m->position = (Vector2){ TILE_SIZE * 10.5f, TILE_SIZE * 1.5f  };
            k->position = (Vector2){ TILE_SIZE * 1.5f,  TILE_SIZE * 1.5f  };
            break;
        case 5:
            p->position = (Vector2){ TILE_SIZE * 3.5f,  TILE_SIZE * 10.5f };
            m->position = (Vector2){ TILE_SIZE * 1.5f,  TILE_SIZE * 1.5f  };
            k->position = (Vector2){ TILE_SIZE * 8.5f,  TILE_SIZE * 6.5f  };
            break;
    }
}

RayHit CastRay(Vector2 start, float angle, int map[MAP_HEIGHT][MAP_WIDTH], bool hasKey) {

    // start the ray at player pos
    float rayX = start.x;
    float rayY = start.y;
    
    // angled step for ray increment
    float stepX = cosf(angle); 
    float stepY = sinf(angle);
    
    // distance traveled by ray
    float distance = 0; 

    // tile type hit by ray
    int tileHit = 0;
    
    // increment ray until it hits a wall or door, or goes out of bounds
    while (distance < (MAX_DEPTH * TILE_SIZE)) {

        rayX += stepX; 
        rayY += stepY; 
        distance += 1.0f; // distance counter

        // ray pos to map cell coords by scaling and truncating
        int mapX = (int)(rayX / TILE_SIZE);
        int mapY = (int)(rayY / TILE_SIZE);
        
        // check if ray is within map bounds
        if (mapX >= 0 && mapX < MAP_WIDTH && mapY >= 0 && mapY < MAP_HEIGHT) {

            // if ray hits wall or door, break and return hit info
            if (isWall(mapX, mapY, map, hasKey) || map[mapY][mapX] == DOOR_TILE) {

                // set tile type hit
                tileHit = map[mapY][mapX];
                break;
            }
        }
    }       
    // determine if hit was vertical or horizontal for texturing
    bool isHitVertical = (fmodf(rayX, TILE_SIZE) < 1.1f || fmodf(rayX, TILE_SIZE) > TILE_SIZE - 1.1f);
    
    // return ray hit object
    return (RayHit){distance, isHitVertical, rayX, rayY, tileHit};
}

void DrawSprite3D(Vector2 pos, Player player, Texture2D tex, float scale, Color color, float depthBuffer[SCREEN_WIDTH]) {

    // distance from player to sprite
    float dx = pos.x - player.position.x;
    float dy = pos.y - player.position.y;

    // angle from player to sprite
    float angle = atan2f(dy, dx) - player.angle;

    // clamp angle to -PI and PI
    if (angle < -PI) angle += 2 * PI;
    if (angle > PI) angle -= 2 * PI;

    // only draw if sprite is within FOV
    if (fabs(angle) < FOV) {

        // distance * scaling factor for perpendicular distance
        float distance = sqrtf(dx*dx + dy*dy) * cosf(angle);

        // prevent division by zero
        if (distance < 0.1f) distance = 0.1f;

        // calculate sprite x location on screen (sprite's center)
        int screenX = (int)((angle / FOV + 0.5f) * SCREEN_WIDTH);

        // only draw if sprite is closer than wall at this x (depth buffer)
        if (screenX >= 0 && screenX < SCREEN_WIDTH && depthBuffer[screenX] > distance) {

            // calculate sprite height on screen based on distance
            float spriteHeight = (SCREEN_HEIGHT * TILE_SIZE) / distance * scale;

            // texture, , 
            DrawTexturePro(tex, (Rectangle){0,0,(float)tex.width,(float)tex.height}, 
                           (Rectangle){(float)screenX - (spriteHeight/2), (SCREEN_HEIGHT/2.0f) - (spriteHeight/2), spriteHeight, spriteHeight}, 
                           (Vector2){0,0}, 0.0f, color);
        }
    }
}

int main(void) {
    int allLevels[5][MAP_HEIGHT][MAP_WIDTH] = {
    // level 1
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1},
        {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    },

    // level 2
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1},
        {1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1},
        {1, 0, 0, 0, 1, 0, 2, 1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    },

    // level 3
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 2, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    },

    // level 4
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
        {1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1},
        {1, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    },

    // level 5
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 1},
        {1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1},
        {1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1},
        {1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    }
};

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib - Maze Portal");
    InitAudioDevice();

    // audio
    Music ambiance = LoadMusicStream("audio/ambiance.mp3");
    Sound jumpscareSound = LoadSound("audio/jumpscare.mp3");
    Sound levelUpSound = LoadSound("audio/levelup.mp3"); 
    
    SetSoundVolume(jumpscareSound, 1.0f);
    SetSoundVolume(levelUpSound, 0.8f); 
    SetMusicVolume(ambiance, 1.0f);
    PlayMusicStream(ambiance);

    // textures
    Texture2D wallTex = LoadTexture("imgs/Walls.png"); 
    Texture2D doorTex = LoadTexture("imgs/Door.png"); 
    Texture2D monsterTex = LoadTexture("imgs/Monster.png"); 
    Texture2D keyTex = LoadTexture("imgs/OrangeBook.png"); 
    Texture2D jumpscareTex = LoadTexture("imgs/jumpscare.png"); 
    Image whiteImg = GenImageColor(64, 64, (Color){200, 200, 200, 255}); // white portal
    Texture2D portalTex = LoadTextureFromImage(whiteImg); // load to GPU
    UnloadImage(whiteImg); // unload from CPU

    // default game state
    GameState state = {0};
    Player player = { .angle = PI / 2.0f, .speed = 180.0f, .rotSpeed = 0.5f, .hasKey = false };
    Monster enemy = { .speed = 140.0f, .active = true };
    Key mazeKey = {0, true};
    
    int currentLevel = 1;
    LoadLevel(currentLevel, &player, &enemy, &mazeKey, &state, allLevels);

    DisableCursor();
    SetTargetFPS(60);

    // game loop
    while (!WindowShouldClose()) {
        // time between frame
        float dt = GetFrameTime();

        // update game logic if not game over
        if (!state.gameOver) {
            UpdateMusicStream(ambiance);

            // handle player input for rotation and movement
            Vector2 mouseDelta = GetMouseDelta();
            player.angle += mouseDelta.x * player.rotSpeed * dt;

            // direction from WASD input
            Vector2 moveVec = {0,0};
            if (IsKeyDown(KEY_W)) moveVec.x += 1; 
            if (IsKeyDown(KEY_S)) moveVec.x -= 1;
            if (IsKeyDown(KEY_D)) moveVec.y += 1; 
            if (IsKeyDown(KEY_A)) moveVec.y -= 1;

            if (Vector2Length(moveVec) > 0) {

                moveVec = Vector2Normalize(moveVec); // clamp at 1
                
                // WASD direction + initial player angle
                float moveAngle = atan2f(moveVec.y, moveVec.x) + player.angle;

                // calculate next position
                Vector2 next = {player.position.x + cosf(moveAngle)*player.speed*dt, 
                                player.position.y + sinf(moveAngle)*player.speed*dt};

                // check collisions separately for x and y to allow sliding along walls
                if (!isWall((int)(next.x/TILE_SIZE), 
                            (int)(player.position.y/TILE_SIZE), state.map, player.hasKey)) 
                            player.position.x = next.x;

                if (!isWall((int)(player.position.x/TILE_SIZE),
                            (int)(next.y/TILE_SIZE), state.map, player.hasKey)) 
                            player.position.y = next.y;
            }

            // move monster towards player
            Vector2 mDir = Vector2Normalize(Vector2Subtract(player.position, enemy.position));

            // calculate monster's next position
            Vector2 mNext = {enemy.position.x + mDir.x * enemy.speed * dt, 
                             enemy.position.y + mDir.y * enemy.speed * dt};
                        
            // check collisions separately for x and y to allow sliding along walls
            if (!isWall((int)(mNext.x/TILE_SIZE), 
                        (int)(enemy.position.y/TILE_SIZE), state.map, player.hasKey)) 
                            enemy.position.x = mNext.x;
            if (!isWall((int)(enemy.position.x/TILE_SIZE),
                        (int)(mNext.y/TILE_SIZE), state.map, player.hasKey)) 
                            enemy.position.y = mNext.y;
            
            // check if player picks up key
            if (mazeKey.active && Vector2Distance(player.position, mazeKey.position) < 32.0f) {
                mazeKey.active = false; player.hasKey = true;
            }

            // player position in map cell coordinates 
            int pX = (int)(player.position.x / TILE_SIZE);
            int pY = (int)(player.position.y / TILE_SIZE);

            // if player has key and is on door tile, go to next level
            if (player.hasKey && state.map[pY][pX] == 2) {
                PlaySound(levelUpSound);
                currentLevel++;
                if (currentLevel > 5) break; 
                LoadLevel(currentLevel, &player, &enemy, &mazeKey, &state, allLevels);
            }

            // check if monster catches player
            if (Vector2Distance(player.position, enemy.position) < 30.0f) {
                state.gameOver = true;
                StopMusicStream(ambiance);
                PlaySound(jumpscareSound);
            }
        // monster caught player, start jumpscare duration    
        } else {
            state.gameOverTimer += dt;
            if (state.gameOverTimer > 2.0f) break; 
        }

        // drawing
        BeginDrawing();
            ClearBackground(BLACK);
            if (!state.gameOver) {
                DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2, (Color){24,24,24,255}, BLACK); 
                DrawRectangleGradientV(0, SCREEN_HEIGHT/2, SCREEN_WIDTH, SCREEN_HEIGHT/2, BLACK, (Color){22,22,22,255}); 

                // create rays and draw walls
                for (int x = 0; x < NUM_RAYS; x++) {

                    // calculate ray angle for current column
                    float rayAngle = (player.angle - FOV/2.0f) + ((float)x/NUM_RAYS)*FOV;

                    // create ray and get hit info
                    RayHit hit = CastRay(player.position, rayAngle, state.map, player.hasKey);

                    // correct distance for fisheye effect
                    // distance * perpendicular dist of angle of ray relative to fov center
                    float correctedDist = hit.distance * cosf(rayAngle - player.angle);
                    // set x pixel on screen to distance of sprite
                    state.depthBuffer[x] = correctedDist;

                    // calculate line height based on distance (closer == taller)
                    float lineHeight = (SCREEN_HEIGHT * TILE_SIZE) / correctedDist;

                    // select texture based on tile type
                    Texture2D activeTex = (hit.tileHit == 2) ? (player.hasKey ? portalTex : doorTex) : wallTex;

                    // calculate x coordinate on texture for vertical slicing
                    float texX = hit.hitVertical ? fmodf(hit.wallYHit, TILE_SIZE) : fmodf(hit.wallXHit, TILE_SIZE);

                    // calculate fade based on distance (farther == more faded)
                    float fade = fmaxf(0, 1.0f - (correctedDist / (MAX_DEPTH * TILE_SIZE)));

                    // if hit door and player has key, tint white, else fade to black based on distance
                    Color tint = (hit.tileHit == 2 && player.hasKey) ? WHITE : (Color){255*fade, 255*fade, 255*fade, 255};

                    // draw wall using sliced texture
                    DrawTexturePro(activeTex, // texture source
                                  (Rectangle){(texX/TILE_SIZE)*activeTex.width, 0, 1, (float)activeTex.height}, // vertical slice texture
                                  (Rectangle){(float)x, (SCREEN_HEIGHT/2.0f)-(lineHeight/2.0f), 1, lineHeight}, // location and size on screen
                                  (Vector2){0,0}, 0.0f, tint); // no rotation, tint based on distance
                }

                // draw sprites (key and monster)
                if (mazeKey.active) 
                    DrawSprite3D(mazeKey.position, player, keyTex, 0.5f, WHITE, state.depthBuffer);
                
                DrawSprite3D(enemy.position, player, monsterTex, 1.0f, WHITE, state.depthBuffer);

                // level number
                DrawText(TextFormat("LEVEL %d", currentLevel), 10, 40, 20, RAYWHITE);
                
                // instructions
                if (!player.hasKey) DrawText("GET THE ORANGE BOOK TO ACTIVATE THE PORTAL", 10, 70, 20, ORANGE);
                else DrawText("PORTAL ACTIVE! WALK INTO THE WHITE LIGHT", 10, 70, 20, WHITE);
            
                // game over, show jumpscare
            } else {
                DrawTexturePro(jumpscareTex, 
                              (Rectangle){0,0,jumpscareTex.width, jumpscareTex.height}, // source
                              (Rectangle){0,0, SCREEN_WIDTH, SCREEN_HEIGHT}, // destination
                              (Vector2){0,0}, 0.0f, RED); // red tint
            }
        EndDrawing();
    }

    // uninitialize resources
    UnloadTexture(wallTex); 
    UnloadTexture(doorTex); 
    UnloadTexture(monsterTex); 
    UnloadTexture(keyTex); 
    UnloadTexture(portalTex); 
    UnloadTexture(jumpscareTex);
    UnloadMusicStream(ambiance); 
    UnloadSound(jumpscareSound); 
    UnloadSound(levelUpSound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
