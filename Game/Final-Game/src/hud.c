#include "hud.h"

void DrawHUD(int currentLevel, bool hasKey)
{
    // Current level
    DrawText(TextFormat("LEVEL %d", currentLevel), 10, 40, 20, RAYWHITE);

    // Key statues and instruction 
    if (!hasKey)
        DrawText("GET THE ORANGE BOOK TO ACTIVATE THE PORTAL", 10, 70, 20, ORANGE);
    else
        DrawText("PORTAL ACTIVE! WALK INTO THE WHITE LIGHT", 10, 70, 20, WHITE);
}

void DrawJumpscare(Texture2D jumpscareTex)
{
    DrawTexturePro(
        jumpscareTex,
        (Rectangle){ 0, 0, (float)jumpscareTex.width, (float)jumpscareTex.height },
        (Rectangle){ 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT },
        (Vector2){ 0, 0 }, 0.0f, RED);
}

void DrawMenuScreen(Texture2D backgroundTex, Texture2D startBTNTex, Texture2D instructTex, Texture2D quitTex)
{

    // Background
    Rectangle source = { 0, 0, (float)backgroundTex.width, (float)backgroundTex.height };
    Rectangle dest = { 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
    DrawTexturePro(backgroundTex, source, dest, (Vector2){0,0}, 0.0f, WHITE);

    // Buttons
    Rectangle startDest = { 520, 330, (float)startBTNTex.width, (float)startBTNTex.height }; // Using same size for all
    Rectangle instructDest = { 520, 430, (float)instructTex.width, (float)instructTex.height };
    Rectangle quitDest = { 520, 530, (float)quitTex.width, (float)quitTex.height };

    // Hover effect
    Vector2 mousePos = GetMousePosition();
    bool hoverStart = CheckCollisionPointRec(mousePos, startDest);
    bool hoverInstruct = CheckCollisionPointRec(mousePos, instructDest);
    bool hoverQuit = CheckCollisionPointRec(mousePos, quitDest);

    // Draw Buttons
    DrawTexturePro(startBTNTex, (Rectangle){0,0,startBTNTex.width, instructTex.height}, 
                                startDest, (Vector2){0,0}, 0.0f, 
                                hoverStart ? GRAY : WHITE);
    DrawTexturePro(instructTex, (Rectangle){0,0,instructTex.width, instructTex.height},
                                instructDest, (Vector2){0,0}, 0.0f, 
                                hoverInstruct ? GRAY : WHITE);
    DrawTexturePro(quitTex, (Rectangle){0,0,quitTex.width, quitTex.height}, 
                                quitDest, (Vector2){0,0}, 0.0f, 
                                hoverQuit ? GRAY : WHITE);
}

void DrawGameOverScreen(Texture2D gameOverTex, Texture2D resumeTex)
{
    DrawTexturePro(gameOverTex, 
                    (Rectangle){ 0, 0, (float)gameOverTex.width, (float)gameOverTex.height }, 
                    (Rectangle){ 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT }, 
                    (Vector2){0,0}, 0.0f, WHITE);

    // Hover effect
    Vector2 mousePos = GetMousePosition();
    Rectangle resumeDest = { 520, 430, (float)resumeTex.width, (float)resumeTex.height };

    bool isHovered = CheckCollisionPointRec(mousePos, resumeDest);
    
    // Draw resume button
    DrawTexturePro(resumeTex,
                    (Rectangle){0,0,resumeTex.width, resumeTex.height},
                    resumeDest,
                    (Vector2){0,0},
                    0.0f,
                    isHovered ? GRAY : WHITE);
}

void DrawInstructionsScreen(Texture2D backgroundTex, Texture2D resumeTex)
{
    // Draw background
    DrawTexturePro(backgroundTex, 
                    (Rectangle){ 0, 0, (float)backgroundTex.width, (float)backgroundTex.height }, 
                    (Rectangle){ 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT }, 
                    (Vector2){0,0}, 0.0f, WHITE);
    
    // Draw rectangular background for better text visibility
    DrawRectangle(50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100, ColorAlpha(BLACK, 0.8f));
    
    // Draw instructions text
    DrawText("INSTRUCTIONS", 350, 100, 40, YELLOW);  
    DrawText("Use WASD to move and mouse to look around", 200, 200, 20, RAYWHITE);
    DrawText("Find the orange book to activate the portal", 200, 250, 20, RAYWHITE);
    DrawText("Avoid the monster and find the exit!", 200, 300, 20, RAYWHITE);
    DrawText("Escape level 5 to win!", 200, 350, 20, RAYWHITE);
    DrawText("Click the resume button to go back to the main menu", 200, 400, 20, RAYWHITE);

    // Hover effect
    Vector2 mousePos = GetMousePosition();
    Rectangle resumeDest = { 520, 530, (float)resumeTex.width, (float)resumeTex.height };

    bool isHovered = CheckCollisionPointRec(mousePos, resumeDest);
    
    // Draw quit button
    DrawTexturePro(resumeTex,
                    (Rectangle){0,0,resumeTex.width, resumeTex.height},
                    resumeDest,
                    (Vector2){0,0},
                    0.0f,
                    isHovered ? GRAY : WHITE);
}