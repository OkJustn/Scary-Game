#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define MAP_WIDTH 12
#define MAP_HEIGHT 12
#define TILE_SIZE 64.0f // size of one wall block

#define FOV (60.0f * DEG2RAD) // field of View
#define NUM_RAYS SCREEN_WIDTH // one ray per pixel column
#define MAX_DEPTH 5.0f // max distance to shade    

// depth buffer to prevent monster from showing through walls
float depthBuffer[SCREEN_WIDTH];

// Global key state for collision logic
bool playerHasKey = false;

// 0 = empty space, 1 = wall, 2 = door/portal
int map[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,0,1,1,1,0,0,1},
    {1,0,1,0,0,0,1,0,0,0,0,1},
    {1,0,1,0,1,1,1,0,1,1,0,1},
    {1,0,0,0,1,0,0,0,1,0,0,1},
    {1,0,1,1,1,0,1,1,1,0,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,2,1,1,1,1,1,1,0,1}, // The '2' here is the portal
    {1,0,0,0,0,0,1,0,0,0,0,1},
    {1,0,1,1,1,0,0,0,1,1,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1}
};

// Second level layout
int mapLevel2[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,1,1,0,1},
    {1,0,1,0,0,0,0,0,0,1,0,1},
    {1,0,1,0,1,1,1,1,0,1,0,1},
    {1,0,1,0,1,0,0,1,0,1,0,1},
    {1,0,1,0,1,0,0,1,0,1,0,1},
    {1,0,1,0,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1}
};

// player struct
typedef struct Player {
    Vector2 position; // struct with x and y floats
    float angle; 
    float speed;
    float rotSpeed; // for mouse sensitivity
} Player;

typedef struct Monster {
    Vector2 position;
    float speed;
    bool active;
} Monster;

// Key struct
typedef struct Key {
    Vector2 position;
    bool active;
} Key;

// check if a map coordinate is a wall
bool isWall(int x, int y) {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return true; 
    
    // NEW: The portal (2) only blocks you IF you don't have the key.
    // This allows the player to "walk into" the portal to change levels.
    if (map[y][x] == 2 && !playerHasKey) return true;

    return (map[y][x] == 1); 
}

void updatePlayer(Player *player, float dt) {
    Vector2 mouseDelta = GetMouseDelta(); 
    player->angle += mouseDelta.x * player->rotSpeed * dt;

    Vector2 moveVec = {0, 0}; 
    if (IsKeyDown(KEY_W)) moveVec.x += 1;
    if (IsKeyDown(KEY_S)) moveVec.x -= 1;
    if (IsKeyDown(KEY_D)) moveVec.y += 1;
    if (IsKeyDown(KEY_A)) moveVec.y -= 1;

    if (Vector2Length(moveVec) > 0) {
        moveVec = Vector2Normalize(moveVec); 
        float moveAngle = atan2f(moveVec.y, moveVec.x) + player->angle;
        
        Vector2 nextPos;
        nextPos.x = player->position.x + (cosf(moveAngle) * player->speed * dt);
        nextPos.y = player->position.y + (sinf(moveAngle) * player->speed * dt);

        int mapX = (int)(nextPos.x / TILE_SIZE); 
        int mapY = (int)(player->position.y / TILE_SIZE); 
        if (!isWall(mapX, mapY)) player->position.x = nextPos.x; 

        mapX = (int)(player->position.x / TILE_SIZE); 
        mapY = (int)(nextPos.y / TILE_SIZE); 
        if (!isWall(mapX, mapY)) player->position.y = nextPos.y; 
    }
}

void updateMonster(Monster *monster, Player player, float dt) {
    if (!monster->active) return;
    Vector2 dir = Vector2Subtract(player.position, monster->position);
    float distToPlayer = Vector2Length(dir);
    if (distToPlayer > 20.0f) { 
        dir = Vector2Normalize(dir);
        Vector2 nextPos;
        nextPos.x = monster->position.x + dir.x * monster->speed * dt;
        nextPos.y = monster->position.y + dir.y * monster->speed * dt;
        int mapX = (int)(nextPos.x / TILE_SIZE); 
        int mapY = (int)(monster->position.y / TILE_SIZE); 
        if (!isWall(mapX, mapY)) monster->position.x = nextPos.x; 
        mapX = (int)(monster->position.x / TILE_SIZE); 
        mapY = (int)(nextPos.y / TILE_SIZE); 
        if (!isWall(mapX, mapY)) monster->position.y = nextPos.y; 
    }
}

typedef struct RayHit {
    float distance;
    bool hitVertical; 
    float wallXHit;   
    float wallYHit;   
    int tileHit;      
} RayHit;

RayHit CastRay(Vector2 start, float angle) {
    float rayX = start.x;
    float rayY = start.y;
    float stepX = cosf(angle); 
    float stepY = sinf(angle);
    float distance = 0;
    int tileHit = 0;
    bool hitVertical = false;

    while (distance < (MAX_DEPTH * TILE_SIZE)) {
        rayX += stepX;
        rayY += stepY;
        distance += 1.0f; 
        int mapX = (int)(rayX / TILE_SIZE); 
        int mapY = (int)(rayY / TILE_SIZE);

        if (isWall(mapX, mapY) || (map[mapY][mapX] == 2)) {
            tileHit = map[mapY][mapX]; 
            float xInTile = fmodf(rayX, TILE_SIZE);
            if (xInTile < 1.1f || xInTile > TILE_SIZE - 1.1f) hitVertical = true;
            else hitVertical = false;
            break;
        }
    }
    RayHit result = {distance, hitVertical, rayX, rayY, tileHit};
    return result;
}

void DrawKey3D(Key key, Player player, Texture2D keyTex) {
    if (!key.active) return;
    float dx = key.position.x - player.position.x;
    float dy = key.position.y - player.position.y;
    float keyAngle = atan2f(dy, dx);
    float relativeAngle = keyAngle - player.angle;
    if (relativeAngle < -PI) relativeAngle += 2 * PI;
    if (relativeAngle > PI) relativeAngle -= 2 * PI;

    if (fabs(relativeAngle) < (FOV)) {
        float distance = sqrtf(dx*dx + dy*dy);
        if (distance < 0.1f) distance = 0.1f;
        float spriteHeight = ((SCREEN_HEIGHT * TILE_SIZE) / distance)/2;
        float screenX = (relativeAngle / FOV + 0.5f) * SCREEN_WIDTH;
        float screenY = (SCREEN_HEIGHT / 2.0f) - (spriteHeight / 2.0f);
        if ((int)screenX >= 0 && (int)screenX < SCREEN_WIDTH && depthBuffer[(int)screenX] < distance) return;
        float fade = fmaxf(0, 1.0f - (distance / (MAX_DEPTH * TILE_SIZE)));
        DrawTexturePro(keyTex, (Rectangle){0,0,(float)keyTex.width,(float)keyTex.height}, (Rectangle){screenX - (spriteHeight/2), screenY, spriteHeight, spriteHeight}, (Vector2){0,0}, 0.0f, (Color){255*fade,255*fade,255*fade,255});
    }
}

void DrawMonster3D(Monster monster, Player player, Texture2D monsterTex) {
    if (!monster.active) return;
    float dx = monster.position.x - player.position.x;
    float dy = monster.position.y - player.position.y;
    float monsterAngle = atan2f(dy, dx);
    float relativeAngle = monsterAngle - player.angle;
    while (relativeAngle < -PI) relativeAngle += 2 * PI;
    while (relativeAngle > PI) relativeAngle -= 2 * PI;

    if (fabs(relativeAngle) < (FOV)) {
        float distance = sqrtf(dx*dx + dy*dy) * cosf(relativeAngle);
        if (distance < 0.1f) distance = 0.1f;
        float spriteHeight = (SCREEN_HEIGHT * TILE_SIZE) / distance;
        float screenX = (relativeAngle / FOV + 0.5f) * SCREEN_WIDTH;
        float screenY = (SCREEN_HEIGHT / 2.0f) - (spriteHeight / 2.0f);
        if ((int)screenX >= 0 && (int)screenX < SCREEN_WIDTH && depthBuffer[(int)screenX] < distance) return;
        float fade = fmaxf(0, 1.0f - (distance / (MAX_DEPTH * TILE_SIZE)));
        DrawTexturePro(monsterTex, (Rectangle){0,0,(float)monsterTex.width,(float)monsterTex.height}, (Rectangle){screenX - (spriteHeight/2), screenY, spriteHeight, spriteHeight}, (Vector2){0,0}, 0.0f, (Color){255*fade,255*fade,255*fade,255});
    }
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib - Maze Portal");

    Texture2D wallTex = LoadTexture("imgs/Walls.png");
    Texture2D doorTex = LoadTexture("imgs/Door.png");
    Texture2D monsterTex = LoadTexture("imgs/Monster.png");
    Texture2D keyTex = LoadTexture("imgs/OrangeBook.png");
    
    // Create a plain white texture for the "Portal" effect
    Image whiteImg = GenImageColor(64, 64, (Color) {200, 200, 200, 255});
    Texture2D portalTex = LoadTextureFromImage(whiteImg);
    UnloadImage(whiteImg);

    Player player = { .position = {TILE_SIZE * 1.5f, TILE_SIZE * 1.5f}, .angle = PI / 2.0f, .speed = 180.0f, .rotSpeed = 0.5f };
    Monster enemy = { .position = {TILE_SIZE * 1.5f, TILE_SIZE * 6.5f}, .speed = 150.0f, .active = true };
    Key mazeKey = { .position = {TILE_SIZE * 10.5f, TILE_SIZE * 1.5f}, .active = true };
    
    int currentLevel = 1;
    DisableCursor(); 
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        updatePlayer(&player, dt);
        updateMonster(&enemy, player, dt);

        if (mazeKey.active && Vector2Distance(player.position, mazeKey.position) < 32.0f) {
            mazeKey.active = false;
            playerHasKey = true;
        }

        // NEW: Check for level transition when standing inside the portal
        int pX = (int)(player.position.x / TILE_SIZE);
        int pY = (int)(player.position.y / TILE_SIZE);
        if (playerHasKey && map[pY][pX] == 2) {
            currentLevel = 2;
            playerHasKey = false;
            player.position = (Vector2){TILE_SIZE * 1.5f, TILE_SIZE * 1.5f};
            enemy.position = (Vector2){TILE_SIZE * 10.5f, TILE_SIZE * 10.5f};
            mazeKey.active = false; 
            for(int y = 0; y < MAP_HEIGHT; y++)
                for(int x = 0; x < MAP_WIDTH; x++)
                    map[y][x] = mapLevel2[y][x];
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2, (Color) {24, 24, 24, 255}, BLACK); 
        DrawRectangleGradientV(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2, BLACK, (Color) {22, 22, 22, 255}); 

        float startAngle = player.angle - (FOV / 2.0f);
        for (int x = 0; x < NUM_RAYS; x++) {
            float rayAngle = startAngle + ((float)x / (float)NUM_RAYS) * FOV;
            RayHit hit = CastRay(player.position, rayAngle);
            float correctedDist = hit.distance * cosf(rayAngle - player.angle);
            depthBuffer[x] = correctedDist;
            if (correctedDist < 0.1f) correctedDist = 0.1f;
            float lineHeight = (SCREEN_HEIGHT * TILE_SIZE) / correctedDist;
            float lineStart = (SCREEN_HEIGHT / 2.0f) - (lineHeight / 2.0f);
            
            // NEW: Portal visual logic
            Texture2D activeTex = wallTex;
            Color tint = WHITE;
            
            if (hit.tileHit == 2) {
                // If the player has the key, make the wall white and "glowing"
                activeTex = playerHasKey ? portalTex : doorTex;
                if (playerHasKey) tint = (Color){255, 255, 255, 255}; // Full bright white
            }

            float texX = hit.hitVertical ? fmodf(hit.wallYHit, TILE_SIZE) : fmodf(hit.wallXHit, TILE_SIZE);
            float textureXPos = (texX / TILE_SIZE) * activeTex.width;
            
            float fade = fmaxf(0, 1.0f - (correctedDist / (MAX_DEPTH * TILE_SIZE)));
            // Don't fade the portal too much so it stays "distinct"
            if (hit.tileHit != 2 || !playerHasKey) {
                tint.r *= fade; tint.g *= fade; tint.b *= fade;
            }

            DrawTexturePro(activeTex, (Rectangle){textureXPos, 0, 1, (float)activeTex.height}, (Rectangle){(float)x, lineStart, 1, lineHeight}, (Vector2){0,0}, 0.0f, tint);
        }

        DrawKey3D(mazeKey, player, keyTex);
        DrawMonster3D(enemy, player, monsterTex);

        DrawText(TextFormat("LEVEL %d", currentLevel), 10, 40, 20, RAYWHITE);
        if (!playerHasKey && currentLevel == 1) DrawText("GET THE ORANGE BOOK TO ACTIVATE THE PORTAL", 10, 70, 20, ORANGE);
        else if (playerHasKey) DrawText("PORTAL ACTIVE! WALK INTO THE WHITE LIGHT", 10, 70, 20, WHITE);

        EndDrawing();
        if (Vector2Distance(player.position, enemy.position) < 20.0f) break;
    }

    UnloadTexture(wallTex); UnloadTexture(doorTex); UnloadTexture(monsterTex); UnloadTexture(keyTex); UnloadTexture(portalTex);
    CloseWindow();
    return 0;
}