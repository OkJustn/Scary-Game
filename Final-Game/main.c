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
    // Initialize window and audio 
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tenacity");
    InitAudioDevice();
    SetExitKey(KEY_NULL);


    // Audio
    AudioAssets audio = LoadAudioAssets();

    // Textures
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

    // Portal texture
    Image     whiteImg  = GenImageColor(64, 64, (Color){ 200, 200, 200, 255 });
    Texture2D portalTex = LoadTextureFromImage(whiteImg);
    UnloadImage(whiteImg);

    // Game objects' state 
    GameState state  = { 0 };
    Player     player = { .angle = PI / 2.0f, .speed = 180.0f,
                         .rotSpeed = 0.5f, .hasKey = false };
    Monster   enemy  = { .speed = 150.0f, .active = true };
    Key       mazeKey = { {0}, true };
    // Screen state
    GameScreen currentScreen = MENU;

    // Load first level
    int currentLevel = 1;
    bool keepRunning = true;
   
    DisableCursor();
    SetTargetFPS(60);

    // Main Loop
    while (!WindowShouldClose() && keepRunning)
    {
        // Time between frames
        float dt = GetFrameTime();

        // Mouse pos for button interactions
        Vector2 mousePos = GetMousePosition();

        // Screen State Handling
        switch (currentScreen)
        {
            // Main menu
            case MENU:
            {
                // Bring cursor back for menu
                if (IsCursorHidden()) EnableCursor();
                
                // Start button
                Rectangle startdest = { 520, 330, (float)startBTNTex.width, (float)startBTNTex.height };
                // Check for button hover and click
                if (CheckCollisionPointRec(mousePos, startdest) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    DisableCursor();
                    
                    // Force complete reset of all settings
                    state.gameOver = false;
                    state.gameOverTimer = 0.0f;
                    currentLevel = 1;
                    LoadLevel(currentLevel, &player, &enemy, &mazeKey, &state);
                    player.hasKey = false;
                    mazeKey.active = true;
                    enemy.active = true;
                    player.angle = PI / 2.0f;
                    
                    // Restart the ambient track
                    PlayMusicStream(audio.ambiance);
                    
                    // Set screen to play mode
                    currentScreen = GAME;
                }
                
                // Instructions button
                Rectangle instructdest = { 520, 430, (float)instructTex.width, (float)instructTex.height };
                // Switch to instructions state when button is pressed
                if (CheckCollisionPointRec(mousePos, instructdest) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    currentScreen = INSTRUCT; 
                }

                // Quit button
                Rectangle quitdest = { 520, 530, (float)quitTex.width, (float)quitTex.height };
                // Check for button hover and click
                if (CheckCollisionPointRec(mousePos, quitdest) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    // Quit game
                    keepRunning = false;
                }
            }
            break;

            // Game screen
            case INSTRUCT:
            {
                // Bring cursor back for instructions
                if (IsCursorHidden()) EnableCursor();
                // Back to menu button
                Rectangle backDest = { 520, 530, (float)resumeTex.width, (float)resumeTex.height };
                if (CheckCollisionPointRec(mousePos, backDest) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                currentScreen = MENU;
                }
            }
            break;

            case VICTORY:
            {
                // Bring cursor back for victory screen
                if (IsCursorHidden()) EnableCursor();
                // Back to menu button
                Rectangle menuDest = { 520, 430, (float)resumeTex.width, (float)resumeTex.height };
                if (CheckCollisionPointRec(mousePos, menuDest) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    currentScreen = MENU;
                }
            }
            break;

            case  GAME:
            {
                // Pause
                if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)){
                    currentScreen = PAUSE;
                    EnableCursor();
                }

                // Game logic (updating of states)
                if (!state.gameOver)
                {
                    // Looping ambiance
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

                    // Load next level if player has key and is on door tile
                    if (player.hasKey && state.map[pY][pX] == DOOR_TILE)
                    {
                        PlayLevelUpAudio(&audio);
                        currentLevel++;
                        // Win condition
                        if (currentLevel > NUM_LEVELS) {
                            currentScreen = VICTORY;
                            break;
                        }
                        LoadLevel(currentLevel, &player, &enemy, &mazeKey, &state);
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
                        // Switch to game over screen
                        currentScreen = GAMEOVER; 
                    
                        // Reset timer
                        state.gameOverTimer = 0.0f; 
                        
                        EnableCursor();
                    }
                }
            }
            break;
            
            case PAUSE:
            {
                // Resume game If key is pressed
                if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)) {
                        DisableCursor();
                        currentScreen = GAME;
                }
            }
            break;
            
            default:
            {
                break;
            }
        }
    
        // Draw
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Draw screens
        switch (currentScreen)
        {
            case MENU:
            {
                DrawMenuScreen(backgroundTex, startBTNTex, instructTex, quitTex);
            }
            break;

            case INSTRUCT:
            {
                DrawInstructionsScreen(backgroundTex, resumeTex);
            }
            break;

            case GAME:
            case PAUSE:
            {
                if (!state.gameOver)
                    {
                    // Draw 3D Environment
                    // Background gradient
                    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2,
                                           (Color){ 24, 24, 24, 255 }, BLACK);
                    DrawRectangleGradientV(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2, BLACK,
                                           (Color){ 22, 22, 22, 255 });

                    // Draw walls
                    DrawWalls(player, &state, wallTex, doorTex, portalTex);
                    
                    // Draw sprites (key, monster)
                    if (mazeKey.active)
                    {
                        DrawSprite3D(mazeKey.position, player, keyTex, 0.5f, WHITE, state.depthBuffer);
                    }

                    if (enemy.active)
                    {
                        DrawSprite3D(enemy.position, player, monsterTex, 1.0f, WHITE, state.depthBuffer);
                    }

                    // Draw level stats
                    DrawHUD(currentLevel, player.hasKey);

                    // Display pause text if paused
                    if (currentScreen == PAUSE) {
                        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ColorAlpha(BLACK, 0.5f));
                        DrawText("PAUSED", SCREEN_WIDTH/2 - 100, 250, 60, WHITE);
                    }
                }
                else 
                {
                    DrawJumpscare(jumpscareTex);
                    currentLevel = 1;
                }
            }
            break;

            case VICTORY:
            {
                DrawVictoryScreen(backgroundTex, resumeTex);
            }
            break;

            case GAMEOVER:
            {
                // Enable cursor
                if (IsCursorHidden()) EnableCursor();

                DrawGameOverScreen(gameOverTex, resumeTex);
                
                // Reset game if resume button is clicked
                bool isHovered = CheckCollisionPointRec(mousePos, (Rectangle){ 520, 430, 
                                                                       (float)resumeTex.width, 
                                                                       (float)resumeTex.height });

                // Go back to menu
                if (isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    state.gameOver = false;     // Turn off game over status flag
                    state.gameOverTimer = 0.0f; // Clear the clock out entirely
                    currentScreen = MENU; 
                }

            }
            break;
        }

        EndDrawing();
    }
    // Cleanup
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

    UnloadAudioAssets(&audio);
    CloseAudioDevice();

    CloseWindow();
    return 0;
}