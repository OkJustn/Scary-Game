#ifndef HUD_H
#define HUD_H

#include "types.h"

// Draws level counter and key/portal status text
void DrawHUD(int currentLevel, bool hasKey);

// Draws the full-screen jumpscare image with red tint
void DrawJumpscare(Texture2D jumpscareTex);

#endif // HUD_H
