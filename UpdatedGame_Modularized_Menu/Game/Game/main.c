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
    SetExitKey(KEY_NULL);


    // ── Audio ─────────────────────────────────────────────────────────────────
    AudioAssets audio = LoadAudioAssets();

    // ── Textures ──────────────────────────────────────────────────────────────
    Texture2D wallTex      = LoadTexture("imgs/Walls.png");
    Texture2D doorTex      = LoadTexture("imgs/Door.png");
    Texture2D monsterTex   = LoadTexture("imgs/Monster.png");
    Texture2D keyTex       = LoadTexture("imgs/OrangeBook.png");
    Texture2D jumpscareTex = LoadTexture("imgs/jumpscare.png");
    Texture2D backgroundTex = LoadTexture("imgs/SS.png");    
    Texture2D gameOverTex = LoadTexture("imgs/G-O.png");    
    Texture2D instructTex = LoadTexture("imgs/Instructions_BTTN.png");    
    Texture2D quitTex = LoadTexture("imgs/Quit_BTTN.png");    
    Texture2D resumeTex = LoadTexture("imgs/Resume_BTTN.png");    
    Texture2D startBTNTex = LoadTexture("imgs/Rename_BTTN.png");    
    Texture2D instructBgTex = LoadTexture("imgs/instructions.png");    

    Image     whiteImg  = GenImageColor(64, 64, (Color){ 200, 200, 200, 255 });
    Texture2D portalTex = LoadTextureFromImage(whiteImg);
    UnloadImage(whiteImg);

    // ── Entity State ──────────────────────────────────────────────────────────
    GameState state  = { 0 };
    Player    player = { .angle = PI / 2.0f, .speed = 180.0f,
                         .rotSpeed = 0.5f, .hasKey = false };
    Monster   enemy  = { .speed = 140.0f, .active = true };
    Key       mazeKey = { {0}, true };
    GameScreen currentScreen = MENU;

    int currentLevel = 1;
    bool keepRunning = true;
    LoadLevel(currentLevel, &player, &enemy, &mazeKey, &state, NULL);

    DisableCursor();
    SetTargetFPS(60);

    // ── Main Loop ─────────────────────────────────────────────────────────────
    while (!WindowShouldClose() && keepRunning)
    {
        float dt = GetFrameTime();
        Vector2 mousePos = GetMousePosition();

        switch (currentScreen)
        {
        case MENU:
            if (IsCursorHidden()) EnableCursor();
                
                Rectangle startdest = { 520, 330, (float)startBTNTex.width, (float)startBTNTex.height };
                if (CheckCollisionPointRec(mousePos, startdest) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    DisableCursor();
                    currentScreen = GAME;
                }

                Rectangle instructdest = { 520, 430, (float)instructTex.width, (float)instructTex.height };
                            if (CheckCollisionPointRec(mousePos, instructdest) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                                currentScreen = INSTRUCTIONS;
                            }
                
                Rectangle quitdest = { 520, 530, (float)quitTex.width, (float)quitTex.height };
                if (CheckCollisionPointRec(mousePos, quitdest) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    keepRunning = false;
                }



            break;
        case  GAME:
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)){
                currentScreen = PAUSE;
                EnableCursor();
             }

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
                if (state.gameOverTimer > 2.0f) 
                {
                    currentScreen = GAMEOVER; 
                    
                    state.gameOverTimer = 0.0f; 
                    
                    EnableCursor();
                }
            }
            break;
        case INSTRUCTIONS:
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)) {
                    currentScreen = MENU;
                }
            break;
            
        case PAUSE:
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)) {
                        DisableCursor();
                        currentScreen = GAME;
                    }
            break;
            
        default:
            break;
            }

        


        // ── Draw ──────────────────────────────────────────────────────────────
        BeginDrawing();
        ClearBackground(BLACK);

        switch (currentScreen)
        {
            case MENU:
            {
                if (IsCursorHidden()) EnableCursor();

                Rectangle source = { 0, 0, (float)backgroundTex.width, (float)backgroundTex.height };
                Rectangle dest = { 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
                DrawTexturePro(backgroundTex, source, dest, (Vector2){0,0}, 0.0f, WHITE);

                Rectangle startdest = { 520, 330, (float)startBTNTex.width, (float)startBTNTex.height }; // Using same size for all
                Rectangle instructdest = { 520, 430, (float)instructTex.width, (float)instructTex.height };
                Rectangle quitdest = { 520, 530, (float)quitTex.width, (float)quitTex.height };

                Vector2 mousePos = GetMousePosition();
                bool hoverStart = CheckCollisionPointRec(mousePos, startdest);
                bool hoverInstruct = CheckCollisionPointRec(mousePos, instructdest);
                bool hoverQuit = CheckCollisionPointRec(mousePos, quitdest);

                DrawTexturePro(startBTNTex, (Rectangle){0,0,startBTNTex.width, instructTex.height}, startdest, (Vector2){0,0}, 0.0f, hoverStart ? GRAY : WHITE);
                DrawTexturePro(instructTex, (Rectangle){0,0,instructTex.width, instructTex.height}, instructdest, (Vector2){0,0}, 0.0f, hoverInstruct ? GRAY : WHITE);
                DrawTexturePro(quitTex, (Rectangle){0,0,quitTex.width, quitTex.height}, quitdest, (Vector2){0,0}, 0.0f, hoverQuit ? GRAY : WHITE);
                
                if (hoverStart && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    DisableCursor();
                    currentScreen = GAME;
                }
                if (hoverQuit && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                }
            }
            break;
            case INSTRUCTIONS:
            {
                if (IsCursorHidden()) EnableCursor();
                Rectangle instsource = { 0, 0, (float)instructBgTex.width, (float)instructBgTex.height };
                Rectangle instdest = { 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
                DrawTexturePro(instructBgTex, instsource, instdest, (Vector2){0,0}, 0.0f, WHITE);
                break;
            }
            case GAME:
  
            case PAUSE:
            {
            if (!state.gameOver)
                {
                    // Draw 3D Environment
                    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2, (Color){ 24, 24, 24, 255 }, BLACK);
                    DrawRectangleGradientV(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2, BLACK, (Color){ 22, 22, 22, 255 });

                    DrawWalls(player, &state, wallTex, doorTex, portalTex);
                    if (mazeKey.active)
                        DrawSprite3D(mazeKey.position, player, keyTex, 0.5f, WHITE, state.depthBuffer);
                    DrawSprite3D(enemy.position, player, monsterTex, 1.0f, WHITE, state.depthBuffer);

                    DrawHUD(currentLevel, player.hasKey);

                    if (currentScreen == PAUSE) {
                        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ColorAlpha(BLACK, 0.5f));
                        DrawText("PAUSED", SCREEN_WIDTH/2 - 100, 250, 60, WHITE);
                    }
                }
            else 
            {
                DrawJumpscare(jumpscareTex);
            }
            }

            break;

            case GAMEOVER:
            {
                DrawJumpscare(jumpscareTex);

                if (IsCursorHidden()) EnableCursor();

                DrawTexturePro(gameOverTex, 
                            (Rectangle){ 0, 0, (float)gameOverTex.width, (float)gameOverTex.height }, 
                            (Rectangle){ 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT }, 
                            (Vector2){0,0}, 0.0f, WHITE);

                Vector2 mousePosGO = GetMousePosition();
                Rectangle resumedest = { 520, 430, (float)resumeTex.width, (float)resumeTex.height };
                bool isHovered4 = CheckCollisionPointRec(mousePosGO, resumedest);

                if (isHovered4 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

                    state.gameOver = false;      // Stop the jumpscare logic
                    state.gameOverTimer = 0.0f;  // Reset the clock
                    player.hasKey = false;       // Take away the key
                    mazeKey.active = true;       // Put the key back in the maze
                    
                    LoadLevel(currentLevel, &player, &enemy, &mazeKey, &state, NULL);

                    currentScreen = MENU; 
                }

               
                DrawTexturePro(resumeTex, 
                            (Rectangle){ 0, 0, (float)resumeTex.width, (float)resumeTex.height }, 
                            resumedest, (Vector2){0,0}, 0.0f, isHovered4 ? GRAY : WHITE);
            }
            break;
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
    UnloadTexture(resumeTex);
    UnloadTexture(quitTex);
    UnloadTexture(backgroundTex);
    UnloadTexture(gameOverTex);
    UnloadTexture(instructBgTex);


    UnloadAudioAssets(&audio);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
