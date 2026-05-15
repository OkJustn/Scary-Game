#include "raylib.h"
#define RAYMATH_STATIC_INLINE
#include "raymath.h"

#include "types.h"
#include "map.h"
#include "player.h"
#include "monster.h"
#include "renderer.h"
#include "hud.h"
#include "audio.h"

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib - Maze Portal");
    InitAudioDevice();

    // ── Audio ─────────────────────────────────────────────────────────────────
    AudioAssets audio = LoadAudioAssets();

    // ── Textures ──────────────────────────────────────────────────────────────
    Texture2D wallTex      = LoadTexture("imgs/Walls.png");
    Texture2D doorTex      = LoadTexture("imgs/Door.png");
    Texture2D monsterTex   = LoadTexture("imgs/Monster.png");
    Texture2D keyTex       = LoadTexture("imgs/OrangeBook.png");
    Texture2D jumpscareTex = LoadTexture("imgs/jumpscare.png");

    Image     whiteImg  = GenImageColor(64, 64, (Color){ 200, 200, 200, 255 });
    Texture2D portalTex = LoadTextureFromImage(whiteImg);
    UnloadImage(whiteImg);

    // ── Entity State ──────────────────────────────────────────────────────────
    GameState state  = { 0 };
    Player    player = { .angle = PI / 2.0f, .speed = 180.0f,
                         .rotSpeed = 0.5f, .hasKey = false };
    Monster   enemy  = { .speed = 140.0f, .active = true };
    Key       mazeKey = { 0, true };

    int currentLevel = 1;
    LoadLevel(currentLevel, &player, &enemy, &mazeKey, &state, NULL);

    DisableCursor();
    SetTargetFPS(60);

    // ── Main Loop ─────────────────────────────────────────────────────────────
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        if (!state.gameOver)
        {
            UpdateMusicStream(audio.ambiance);

            // Update entities
            UpdatePlayer(&player, &state, dt);
            UpdateMonster(&enemy, &player, &state, dt);

            // Key pickup
            if (mazeKey.active &&
                Vector2Distance(player.position, mazeKey.position) < 32.0f)
            {
                mazeKey.active = false;
                player.hasKey  = true;
            }

            // Portal / level transition
            int pX = (int)(player.position.x / TILE_SIZE);
            int pY = (int)(player.position.y / TILE_SIZE);
            if (player.hasKey && state.map[pY][pX] == DOOR_TILE)
            {
                PlayLevelUpAudio(&audio);
                currentLevel++;
                if (currentLevel > NUM_LEVELS) break;
                LoadLevel(currentLevel, &player, &enemy, &mazeKey, &state, NULL);
            }

            // Death check
            if (Vector2Distance(player.position, enemy.position) < 30.0f)
            {
                state.gameOver = true;
                PlayGameOverAudio(&audio);
            }
        }
        else
        {
            // Jumpscare timer
            state.gameOverTimer += dt;
            if (state.gameOverTimer > 2.0f) break;
        }

        // ── Draw ──────────────────────────────────────────────────────────────
        BeginDrawing();
        ClearBackground(BLACK);

        if (!state.gameOver)
        {
            // Ceiling / floor gradient
            DrawRectangleGradientV(0, 0,
                SCREEN_WIDTH, SCREEN_HEIGHT / 2,
                (Color){ 24, 24, 24, 255 }, BLACK);
            DrawRectangleGradientV(0, SCREEN_HEIGHT / 2,
                SCREEN_WIDTH, SCREEN_HEIGHT / 2,
                BLACK, (Color){ 22, 22, 22, 255 });

            // 3-D world
            DrawWalls(player, &state, wallTex, doorTex, portalTex);
            if (mazeKey.active)
                DrawSprite3D(mazeKey.position, player, keyTex,
                             0.5f, WHITE, state.depthBuffer);
            DrawSprite3D(enemy.position, player, monsterTex,
                         1.0f, WHITE, state.depthBuffer);

            // HUD
            DrawHUD(currentLevel, player.hasKey);
        }
        else
        {
            DrawJumpscare(jumpscareTex);
        }

        EndDrawing();
    }

    // ── Cleanup ───────────────────────────────────────────────────────────────
    UnloadTexture(wallTex);
    UnloadTexture(doorTex);
    UnloadTexture(monsterTex);
    UnloadTexture(keyTex);
    UnloadTexture(portalTex);
    UnloadTexture(jumpscareTex);
    UnloadAudioAssets(&audio);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
