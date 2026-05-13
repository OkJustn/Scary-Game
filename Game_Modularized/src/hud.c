#include "hud.h"

void DrawHUD(int currentLevel, bool hasKey)
{
    // Draw the current level and key status on the screen
    DrawText(TextFormat("LEVEL %d", currentLevel), 10, 40, 20, RAYWHITE);

    // Display different messages based on whether the player has the key or not
    if (!hasKey)
        DrawText("GET THE ORANGE BOOK TO ACTIVATE THE PORTAL", 10, 70, 20, ORANGE);
    else
        DrawText("PORTAL ACTIVE! WALK INTO THE WHITE LIGHT", 10, 70, 20, WHITE);
}
