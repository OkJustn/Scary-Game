#include "raylib.h"
#include "raymath.h"
#include "types.h"
#include "map.h"
#include "player.h"
#include "monster.h"
#include "renderer.h"
#include "hud.h"

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib - Maze Portal");

    // Load textures
    Texture2D wallTex    = LoadTexture("imgs/Walls.png");
    Texture2D doorTex    = LoadTexture("imgs/Door.png");
    Texture2D monsterTex = LoadTexture("imgs/Monster.png");
    Texture2D keyTex     = LoadTexture("imgs/OrangeBook.png");

    // Generate a plain white texture for the active portal
    Image     whiteImg  = GenImageColor(64, 64, (Color){ 200, 200, 200, 255 });
    Texture2D portalTex = LoadTextureFromImage(whiteImg);
    UnloadImage(whiteImg);

    // Entity state
    Player  player   = { .angle = PI / 2.0f, .speed = 180.0f, .rotSpeed = 0.5f };
    Monster enemy    = { .speed = 0.0f,       .active = true };
    Key     mazeKey  = { 0, true };

    int currentLevel = 1;
    LoadLevel(currentLevel, &player, &enemy, &mazeKey);

    DisableCursor();
    SetTargetFPS(60);

    // ─── Main Loop ────────────────────────────────────────────────────────────
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Update
        UpdatePlayer(&player, dt);
        UpdateMonster(&enemy, &player, dt);

        // Key pickup
        if (mazeKey.active && Vector2Distance(player.position, mazeKey.position) < 32.0f)
        {
            mazeKey.active = false;
            playerHasKey   = true;
        }

        // Portal / level transition
        int pX = (int)(player.position.x / TILE_SIZE);
        int pY = (int)(player.position.y / TILE_SIZE);
        if (playerHasKey && map[pY][pX] == DOOR_TILE)
        {
            currentLevel++;
            if (currentLevel > NUM_LEVELS) break;
            LoadLevel(currentLevel, &player, &enemy, &mazeKey);
        }

        // Death condition
        if (Vector2Distance(player.position, enemy.position) < 20.0f) break;

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);

        // Background gradient (ceiling / floor)
        DrawRectangleGradientV(0, 0,
            SCREEN_WIDTH, SCREEN_HEIGHT / 2,
            (Color){ 24, 24, 24, 255 }, BLACK);
        DrawRectangleGradientV(0, SCREEN_HEIGHT / 2,
            SCREEN_WIDTH, SCREEN_HEIGHT / 2,
            BLACK, (Color){ 22, 22, 22, 255 });

        // 3-D world
        DrawWalls(player, wallTex, doorTex, portalTex);
        if (mazeKey.active)
            DrawSprite3D(mazeKey.position, player, keyTex,     0.5f, WHITE);
        DrawSprite3D(enemy.position, player, monsterTex, 1.0f, WHITE);

        // HUD
        DrawHUD(currentLevel, playerHasKey);

        EndDrawing();
    }

    // Cleanup
    UnloadTexture(wallTex);
    UnloadTexture(doorTex);
    UnloadTexture(monsterTex);
    UnloadTexture(keyTex);
    UnloadTexture(portalTex);
    CloseWindow();
    return 0;
}
