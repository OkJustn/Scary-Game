#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <math.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define MAP_WIDTH 12
#define MAP_HEIGHT 12
#define TILE_SIZE 64.0f 
#define FOV (60.0f * DEG2RAD) 
#define NUM_RAYS SCREEN_WIDTH 
#define MAX_DEPTH 5.0f    
#define DOOR_TILE 2

float depthBuffer[SCREEN_WIDTH];
bool playerHasKey = false;
int map[MAP_HEIGHT][MAP_WIDTH];

// all 5 maps
int allLevels[5][MAP_HEIGHT][MAP_WIDTH] = {
    // level 1
    {
        {1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,0,1,1,1,0,0,1},
        {1,0,1,0,0,0,1,0,0,0,0,1},
        {1,0,1,0,1,1,1,0,1,1,0,1},
        {1,0,0,0,1,0,0,0,1,0,0,1},
        {1,0,1,1,1,0,1,1,1,0,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,2,1,1,1,1,1,1,0,1}, 
        {1,0,0,0,0,0,1,0,0,0,0,1},
        {1,0,1,1,1,0,0,0,1,1,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1}
    },
    // level 2
    {
        {1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,1}, 
        {1,0,1,1,1,1,0,1,1,1,0,1}, 
        {1,0,1,0,0,0,0,0,0,1,0,1}, 
        {1,0,1,0,1,1,1,1,0,1,0,1}, 
        {1,0,0,0,1,0,0,1,0,1,0,1}, 
        {1,1,1,0,1,0,0,1,0,0,0,1}, 
        {1,0,0,0,1,1,0,1,1,1,0,1}, 
        {1,0,1,1,1,0,0,0,0,1,0,1}, 
        {1,0,1,0,0,0,1,1,0,1,0,1}, 
        {1,0,0,0,1,0,2,1,0,0,0,1}, 
        {1,1,1,1,1,1,1,1,1,1,1,1}
    },
    // level 3 
    {
        {1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,1,1,1,1,0,1},
        {1,0,1,0,0,0,0,0,0,1,0,1},
        {1,0,1,0,1,1,1,1,0,1,0,1},
        {1,0,1,0,1,0,0,1,0,1,0,1},
        {1,0,1,0,1,2,0,1,0,1,0,1},
        {1,0,1,0,1,1,0,1,0,1,0,1},
        {1,0,1,0,0,0,0,1,0,1,0,1},
        {1,0,1,1,1,1,1,1,0,1,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1}
    },
    // level 4
    {
        {1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,1,0,0,0,0,1},
        {1,0,1,1,1,0,1,0,1,1,0,1},
        {1,0,0,0,1,0,0,0,1,0,0,1},
        {1,1,1,0,1,1,1,1,1,0,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,1,1,1,1,0,1},
        {1,0,1,0,0,0,0,0,0,1,0,1},
        {1,0,1,0,1,1,1,1,0,1,0,1},
        {1,2,0,0,1,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1,1}
    },
    // level 5
    {
        {1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,1,0,0,0,0,0,0,0,2,1},
        {1,0,1,0,1,1,1,1,0,1,1,1},
        {1,0,0,0,1,0,0,1,0,0,0,1},
        {1,1,1,0,1,0,1,1,1,1,0,1},
        {1,0,0,0,0,0,0,0,0,1,0,1},
        {1,0,1,1,1,0,1,1,0,1,0,1},
        {1,0,1,0,0,0,1,0,0,0,0,1},
        {1,0,1,0,1,1,1,1,1,1,0,1},
        {1,0,0,0,1,0,0,0,0,0,0,1},
        {1,1,1,0,0,0,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1,1}
    }
};

// game object structs
typedef struct Player { 
    Vector2 position; 
    float angle; 
    float speed; 
    float rotSpeed; 
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

typedef struct RayHit { float distance;
    bool hitVertical;
    float wallXHit;
    float wallYHit;
    int tileHit;
} RayHit;

// wall detection
bool isWall(int x, int y) {
    // check out of bounds
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return true; 
    // check if it's a door and player doesn't have the key
    if (map[y][x] == 2 && !playerHasKey) return true;
    // return true if it's a wall
    return (map[y][x] == 1); 
}

// loads a level and sets specific start coordinates each level
void LoadLevel(int levelNum, Player *p, Monster *m, Key *k) {
    // convert to index 0
    int idx = levelNum - 1;

    // ????????????
    for(int y = 0; y < MAP_HEIGHT; y++) {
        for(int x = 0; x < MAP_WIDTH; x++) {
            map[y][x] = allLevels[idx][y][x];
        }
    }
    
    // reset player, monster, and key states
    playerHasKey = false;
    k->active = true;

    // config for each level
    switch(levelNum) {
        case 1:
            p->position = (Vector2){TILE_SIZE * 1.5f, TILE_SIZE * 1.5f}; // player
            m->position = (Vector2){TILE_SIZE * 5.5f, TILE_SIZE * 5.5f}; // monster
            k->position = (Vector2){TILE_SIZE * 10.5f, TILE_SIZE * 1.5f}; // key
            break;
        case 2:
            p->position = (Vector2){TILE_SIZE * 1.5f, TILE_SIZE * 10.5f}; 
            m->position = (Vector2){TILE_SIZE * 1.5f, TILE_SIZE * 1.5f};  
            k->position = (Vector2){TILE_SIZE * 10.5f, TILE_SIZE * 1.5f}; 
            break;
        case 3:
            p->position = (Vector2){TILE_SIZE * 1.5f, TILE_SIZE * 1.5f};  
            m->position = (Vector2){TILE_SIZE * 10.5f, TILE_SIZE * 10.5f}; 
            k->position = (Vector2){TILE_SIZE * 6.5f, TILE_SIZE * 5.5f};  
            break;
        case 4:
            p->position = (Vector2){TILE_SIZE * 5.5f, TILE_SIZE * 5.5f};  
            m->position = (Vector2){TILE_SIZE * 10.5f, TILE_SIZE * 1.5f}; 
            k->position = (Vector2){TILE_SIZE * 1.5f, TILE_SIZE * 1.5f};  
            break;
        case 5:
            p->position = (Vector2){TILE_SIZE * 3.5f, TILE_SIZE * 10.5f}; 
            m->position = (Vector2){TILE_SIZE * 1.5f, TILE_SIZE * 1.5f};  
            k->position = (Vector2){TILE_SIZE * 8.5f, TILE_SIZE * 6.5f};  
            break;
    }
}

RayHit CastRay(Vector2 start, float angle) {
    // start ray at player pos 
    float rayX = start.x; 
    float rayY = start.y;

    // ray step increments
    float stepX = cosf(angle);
    float stepY = sinf(angle);

    // ray length and hit info
    float distance = 0;
    int tileHit = 0;

    // step the ray until it hits a wall or goes out of bounds
    while (distance < (MAX_DEPTH * TILE_SIZE)) {
        rayX += stepX; 
        rayY += stepY; 
        distance += 1.0f; // distance of ray

        // get map coords of ray using scaling and truncation
        int mapX = (int)(rayX / TILE_SIZE);
        int mapY = (int)(rayY / TILE_SIZE);

        // check if ray is within map bounds
        if (mapX >= 0 && mapX < MAP_WIDTH && mapY >= 0 && mapY < MAP_HEIGHT) {

            // check if ray hit a wall or door
            if (isWall(mapX, mapY) || map[mapY][mapX] == DOOR_TILE) {
                // return the tile value (wall or door)
                tileHit = map[mapY][mapX];
                break;
            }
        }
    }       
    // determine if ray hit wall vertically
    bool isHitVertical = (fmodf(rayX, TILE_SIZE) < 1.1f || fmodf(rayX, TILE_SIZE) > TILE_SIZE - 1.1f);
    return (RayHit){distance, isHitVertical, rayX, rayY, tileHit};
}

void DrawSprite3D(Vector2 pos, Player player, Texture2D tex, float scale, Color color) {

    // x and y components from player to sprite
    float dx = pos.x - player.position.x;
    float dy = pos.y - player.position.y;

    // angle from player to sprite, adjusted by player angle
    float angle = atan2f(dy, dx) - player.angle;

    // ensure angle is between 2PI and -2PI
    if (angle < -PI) angle += 2 * PI;
    if (angle > PI) angle -= 2 * PI;

    // only draw if sprite is within FOV
    if (fabs(angle) < FOV) {

        // distance from player to sprite * perpendicular distance correction
        float distance = sqrtf(dx*dx + dy*dy) * cosf(angle);
        
        // prevent division by zero
        if (distance < 0.1f) distance = 0.1f;

        // calculate sprite screen x coordinate !!!!!!!!!!!!!!!!!!!!!!!!
        int screenX = (int)((angle / FOV + 0.5f) * SCREEN_WIDTH);
        
        // only draw if sprite is in front of player and closer than wall
        if (screenX >= 0 && screenX < SCREEN_WIDTH && depthBuffer[screenX] > distance) {

            float spriteHeight = (SCREEN_HEIGHT * TILE_SIZE) / distance * scale;
                                // start rectangle !!!!!!!!!!!!!!
            DrawTexturePro(tex, (Rectangle){0,0,(float)tex.width,(float)tex.height}, 
                            // end rectangle !!!!!!!!!!!!!!!
                           (Rectangle){(float)screenX - (spriteHeight/2), (SCREEN_HEIGHT/2.0f) - (spriteHeight/2), spriteHeight, spriteHeight}, 
                           (Vector2){0,0}, 0.0f, color);
        }
    }
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib - Maze Portal");

    // load textures
    Texture2D wallTex = LoadTexture("imgs/Walls.png");
    Texture2D doorTex = LoadTexture("imgs/Door.png");
    Texture2D monsterTex = LoadTexture("imgs/Monster.png");
    Texture2D keyTex = LoadTexture("imgs/OrangeBook.png");
    Image whiteImg = GenImageColor(64, 64, (Color){200, 200, 200, 255}); // portal color
    Texture2D portalTex = LoadTextureFromImage(whiteImg); // load to gpu
    UnloadImage(whiteImg);

    Player player = { .angle = PI / 2.0f, .speed = 180.0f, .rotSpeed = 0.5f };
    Monster enemy = { .speed = 0.0f, .active = true };
    Key mazeKey = {0, true};
    
    int currentLevel = 1;
    LoadLevel(currentLevel, &player, &enemy, &mazeKey);

    DisableCursor();
    SetTargetFPS(60);

    // main game loop
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // get input and move player
        Vector2 mouseDelta = GetMouseDelta();
        player.angle += mouseDelta.x * player.rotSpeed * dt;
        Vector2 moveVec = {0,0};
        if (IsKeyDown(KEY_W)) moveVec.x += 1; if (IsKeyDown(KEY_S)) moveVec.x -= 1;
        if (IsKeyDown(KEY_D)) moveVec.y += 1; if (IsKeyDown(KEY_A)) moveVec.y -= 1;
        if (Vector2Length(moveVec) > 0) {
            moveVec = Vector2Normalize(moveVec);
            float moveAngle = atan2f(moveVec.y, moveVec.x) + player.angle;
            Vector2 next = {player.position.x + cosf(moveAngle)*player.speed*dt, player.position.y + sinf(moveAngle)*player.speed*dt};
            if (!isWall((int)(next.x/TILE_SIZE), (int)(player.position.y/TILE_SIZE))) player.position.x = next.x;
            if (!isWall((int)(player.position.x/TILE_SIZE), (int)(next.y/TILE_SIZE))) player.position.y = next.y;
        }

        // monster AI
        Vector2 mDir = Vector2Normalize(Vector2Subtract(player.position, enemy.position));
        Vector2 mNext = {enemy.position.x + mDir.x*enemy.speed*dt, enemy.position.y + mDir.y*enemy.speed*dt};
        if (!isWall((int)(mNext.x/TILE_SIZE), (int)(enemy.position.y/TILE_SIZE))) enemy.position.x = mNext.x;
        if (!isWall((int)(enemy.position.x/TILE_SIZE), (int)(mNext.y/TILE_SIZE))) enemy.position.y = mNext.y;

        // key/portal Logic
        if (mazeKey.active && Vector2Distance(player.position, mazeKey.position) < 32.0f) {
            mazeKey.active = false; playerHasKey = true;
        }

        int pX = (int)(player.position.x / TILE_SIZE), pY = (int)(player.position.y / TILE_SIZE);
        if (playerHasKey && map[pY][pX] == 2) {
            currentLevel++;
            if (currentLevel > 5) break; 
            LoadLevel(currentLevel, &player, &enemy, &mazeKey);
        }

        // death condition close game
        if (Vector2Distance(player.position, enemy.position) < 20.0f) break;

        // draw
        BeginDrawing();
            ClearBackground(BLACK);
            DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2, (Color){24,24,24,255}, BLACK); 
            DrawRectangleGradientV(0, SCREEN_HEIGHT/2, SCREEN_WIDTH, SCREEN_HEIGHT/2, BLACK, (Color){22,22,22,255}); 

            for (int x = 0; x < NUM_RAYS; x++) {
                float rayAngle = (player.angle - FOV/2.0f) + ((float)x/NUM_RAYS)*FOV;
                RayHit hit = CastRay(player.position, rayAngle);
                float correctedDist = hit.distance * cosf(rayAngle - player.angle);
                depthBuffer[x] = correctedDist;

                float lineHeight = (SCREEN_HEIGHT * TILE_SIZE) / correctedDist;
                Texture2D activeTex = (hit.tileHit == 2) ? (playerHasKey ? portalTex : doorTex) : wallTex;
                float texX = hit.hitVertical ? fmodf(hit.wallYHit, TILE_SIZE) : fmodf(hit.wallXHit, TILE_SIZE);
                float fade = fmaxf(0, 1.0f - (correctedDist / (MAX_DEPTH * TILE_SIZE)));
                Color tint = (hit.tileHit == 2 && playerHasKey) ? WHITE : (Color){255*fade, 255*fade, 255*fade, 255};

                DrawTexturePro(activeTex, (Rectangle){(texX/TILE_SIZE)*activeTex.width, 0, 1, (float)activeTex.height}, 
                               (Rectangle){(float)x, (SCREEN_HEIGHT/2.0f)-(lineHeight/2.0f), 1, lineHeight}, (Vector2){0,0}, 0.0f, tint);
            }

            if (mazeKey.active) DrawSprite3D(mazeKey.position, player, keyTex, 0.5f, WHITE);
            DrawSprite3D(enemy.position, player, monsterTex, 1.0f, WHITE);

            DrawText(TextFormat("LEVEL %d", currentLevel), 10, 40, 20, RAYWHITE);
            if (!playerHasKey) DrawText("GET THE ORANGE BOOK TO ACTIVATE THE PORTAL", 10, 70, 20, ORANGE);
            else DrawText("PORTAL ACTIVE! WALK INTO THE WHITE LIGHT", 10, 70, 20, WHITE);
        EndDrawing();
    }

    UnloadTexture(wallTex); UnloadTexture(doorTex); UnloadTexture(monsterTex); UnloadTexture(keyTex); UnloadTexture(portalTex);
    CloseWindow();
    return 0;
}
