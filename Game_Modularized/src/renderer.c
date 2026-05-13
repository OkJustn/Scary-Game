#include "renderer.h"
#include "map.h"
#include <math.h>

// ─── Ray Casting ──────────────────────────────────────────────────────────────
RayHit CastRay(Vector2 start, float angle)
{
    float rayX = start.x;
    float rayY = start.y;
    float stepX = cosf(angle);
    float stepY = sinf(angle);

    float distance = 0;
    int tileHit = 0;

    // Incrementally step along the ray until we hit a wall or reach max depth
    while (distance < (MAX_DEPTH * TILE_SIZE))
    {
        rayX += stepX;
        rayY += stepY;
        distance += 1.0f;

        int mapX = (int)(rayX / TILE_SIZE);
        int mapY = (int)(rayY / TILE_SIZE);

        // Check if the ray is out of bounds or hits a wall/door
        if (mapX >= 0 && mapX < MAP_WIDTH && mapY >= 0 && mapY < MAP_HEIGHT)
        {
            if (isWall(mapX, mapY) || map[mapY][mapX] == DOOR_TILE)
            {
                // Record the type of tile hit (wall or door) for rendering purposes
                tileHit = map[mapY][mapX];
                break;
            }
        }
    }

    bool isHitVertical = (fmodf(rayX, TILE_SIZE) < 1.1f ||
                          fmodf(rayX, TILE_SIZE) > TILE_SIZE - 1.1f);

    return (RayHit){distance, isHitVertical, rayX, rayY, tileHit};
}

// ─── Wall / Door Rendering ────────────────────────────────────────────────────
void DrawWalls(Player player, Texture2D wallTex, Texture2D doorTex, Texture2D portalTex)
{
    for (int x = 0; x < NUM_RAYS; x++)
    {
        // Calculate the angle of the ray relative to the player's viewing direction
        float rayAngle = (player.angle - FOV / 2.0f) + ((float)x / NUM_RAYS) * FOV;
        RayHit hit = CastRay(player.position, rayAngle);
        float correctedDist = hit.distance * cosf(rayAngle - player.angle);
        depthBuffer[x] = correctedDist;

        // Calculate the height of the wall slice to draw based on the distance to the hit
        float lineHeight = (SCREEN_HEIGHT * TILE_SIZE) / correctedDist;
        Texture2D activeTex = (hit.tileHit == DOOR_TILE)
                                  ? (playerHasKey ? portalTex : doorTex)
                                  : wallTex;

        // Calculate the x-coordinate on the texture to sample from based on where the ray hit the wall
        float texX = hit.hitVertical
                         ? fmodf(hit.wallYHit, TILE_SIZE)
                         : fmodf(hit.wallXHit, TILE_SIZE);

        // Calculate fade based on distance and apply it to the color tint
        float fade = fmaxf(0.0f, 1.0f - (correctedDist / (MAX_DEPTH * TILE_SIZE)));
        Color tint = (hit.tileHit == DOOR_TILE && playerHasKey)
                         ? WHITE
                         : (Color){(unsigned char)(255 * fade),
                                   (unsigned char)(255 * fade),
                                   (unsigned char)(255 * fade), 255};

        // Draw the vertical slice of the wall/door/portal texture on the screen
        DrawTexturePro(
            activeTex,
            (Rectangle){(texX / TILE_SIZE) * activeTex.width, 0, 1, (float)activeTex.height},
            (Rectangle){(float)x, (SCREEN_HEIGHT / 2.0f) - (lineHeight / 2.0f), 1, lineHeight},
            (Vector2){0, 0}, 0.0f, tint);
    }
}

// ─── Sprite Projection ────────────────────────────────────────────────────────
void DrawSprite3D(Vector2 pos, Player player, Texture2D tex, float scale, Color color)
{
    float dx = pos.x - player.position.x;
    float dy = pos.y - player.position.y;
    float angle = atan2f(dy, dx) - player.angle;

    // Normalise angle to [-PI, PI]
    if (angle < -PI)
        angle += 2.0f * PI;
    if (angle > PI)
        angle -= 2.0f * PI;

    if (fabsf(angle) >= FOV)
        return;

    float distance = sqrtf(dx * dx + dy * dy) * cosf(angle);
    if (distance < 0.1f)
        distance = 0.1f;

    int screenX = (int)((angle / FOV + 0.5f) * SCREEN_WIDTH);

    if (screenX >= 0 && screenX < SCREEN_WIDTH && depthBuffer[screenX] > distance)
    {
        float spriteHeight = (SCREEN_HEIGHT * TILE_SIZE) / distance * scale;

        DrawTexturePro(
            tex,
            (Rectangle){0, 0, (float)tex.width, (float)tex.height},
            (Rectangle){(float)screenX - (spriteHeight / 2.0f),
                        (SCREEN_HEIGHT / 2.0f) - (spriteHeight / 2.0f),
                        spriteHeight, spriteHeight},
            (Vector2){0, 0}, 0.0f, color);
    }
}
