#ifndef HUD_H
#define HUD_H

#include "types.h"
#include "raylib.h"

// Draws level counter and key/portal status text
void DrawHUD(int currentLevel, bool hasKey);

// Draws the full-screen jumpscare image with red tint
void DrawJumpscare(Texture2D jumpscareTex);

// Draw main menu with buttons
void DrawMenuScreen(Texture2D backgroundTex, Texture2D startBTNTex, Texture2D instructTex, Texture2D quitTex);

// Draw game over screen with resume button
void DrawGameOverScreen(Texture2D gameOverTex, Texture2D resumeTex);

void DrawInstructionsScreen(Texture2D backgroundTex, Texture2D resumeTex);
#endif // HUD_H
