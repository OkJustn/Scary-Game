#include "hud.h"

void DrawHUD(int currentLevel, bool hasKey)
{
    DrawText(TextFormat("LEVEL %d", currentLevel), 10, 40, 20, RAYWHITE);

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
