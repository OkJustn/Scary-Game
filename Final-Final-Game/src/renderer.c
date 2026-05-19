#include "renderer.h"
#include "map.h"
#include <math.h>

// Ray Casting 
RayHit CastRay(Vector2 start, float angle,
               int map[MAP_HEIGHT][MAP_WIDTH], bool hasKey)
{
    // Ray starting pos
    float rayX = start.x;
    float rayY = start.y;
    // Ray step increments for x and y
    float stepX = cosf(angle);
    float stepY = sinf(angle);

    // Ray length
    float distance = 0;
    // Ray type hit (0=none, 1=wall, 2=door)
    int   tileHit  = 0;

    // Increment ray until it hits a wall or door, or goes out of bounds
    while (distance < (MAX_DEPTH * TILE_SIZE))
    {
        // Step forward
        rayX     += stepX;
        rayY     += stepY;
        
        // Counter
        distance += 1.0f;

        // Truncate to tile coordinates
        int mapX = (int)(rayX / TILE_SIZE);
        int mapY = (int)(rayY / TILE_SIZE);

        // Check if ray end is within bounds
        if (mapX >= 0 && mapX < MAP_WIDTH && mapY >= 0 && mapY < MAP_HEIGHT)
        {

            // Check if ray hit a wall or door
            if (isWall(mapX, mapY, map, hasKey) || map[mapY][mapX] == DOOR_TILE)
            {
                // return hit info
                tileHit = map[mapY][mapX];
                break;
            }
        }
    }

    // check if x component is inside a wall
    bool isHitVertical = (fmodf(rayX, TILE_SIZE) < 1.1f ||
                          fmodf(rayX, TILE_SIZE) > TILE_SIZE - 1.1f);

    // Return ray object
    return (RayHit){ distance, isHitVertical, rayX, rayY, tileHit };
}

// Wall / Door Columns
void DrawWalls(Player player, GameState *state,
               Texture2D wallTex, Texture2D doorTex, Texture2D portalTex)
{
    // Cast rays of screen width
    for (int x = 0; x < NUM_RAYS; x++)
    {
        // Starting ray angle for current ray
        float  rayAngle      = (player.angle - FOV / 2.0f) + ((float)x / NUM_RAYS) * FOV;
        // Cast ray
        RayHit hit           = CastRay(player.position, rayAngle, state->map, player.hasKey);
       
        // get perpendicular distance to avoid fish-eye distortion
        float  correctedDist = hit.distance * cosf(rayAngle - player.angle);
        state->depthBuffer[x] = correctedDist;

        // Calculate line height based on distance
        float     lineHeight = (SCREEN_HEIGHT * TILE_SIZE) / correctedDist;

        // Choose texture based on hit type
        Texture2D activeTex  = (hit.tileHit == DOOR_TILE)
                                   ? (player.hasKey ? portalTex : doorTex)
                                   : wallTex;

        // Calculate x coordinate for the texture from decimal hit position
        float texX = hit.hitVertical
                         ? fmodf(hit.wallYHit, TILE_SIZE)
                         : fmodf(hit.wallXHit, TILE_SIZE);

        // calculate shade based on distance (closer walls are brighter)
        float fade = fmaxf(0.0f, 1.0f - (correctedDist / (MAX_DEPTH * TILE_SIZE)));

        // If hit is a door and player has key, Color is white, otherwise apply fade for shading
        Color shading = (hit.tileHit == DOOR_TILE && player.hasKey)
                         ? WHITE
                         : (Color){ (unsigned char)(255 * fade),
                                    (unsigned char)(255 * fade),
                                    (unsigned char)(255 * fade), 255 };

        // Draw the active texture column with vertical scaling based on distance with shading
        DrawTexturePro(
            activeTex,
            (Rectangle){ (texX / TILE_SIZE) * activeTex.width, 0, 1, (float)activeTex.height },
            (Rectangle){ (float)x, (SCREEN_HEIGHT / 2.0f) - (lineHeight / 2.0f), 1, lineHeight },
            (Vector2){ 0, 0 }, 0.0f, shading);
    }
}

// Sprite Projection
void DrawSprite3D(Vector2 pos, Player player, Texture2D tex,
                  float scale, Color color, float depthBuffer[SCREEN_WIDTH])
{
    // Calculate vector from player to sprite
    float dx    = pos.x - player.position.x;
    float dy    = pos.y - player.position.y;
    float angle = atan2f(dy, dx) - player.angle;

    // Contain angle to -PI to PI
    if (angle < -PI) angle += 2.0f * PI;
    if (angle >  PI) angle -= 2.0f * PI;

    // Do not draw if sprite is outside FOV
    if (fabsf(angle) >= FOV) return;

    // Calculate perpendicular distance to sprite to avoid fish-eye distortion
    float distance = sqrtf(dx * dx + dy * dy) * cosf(angle);

    // Avoid division by zero due to very close distances
    if (distance < 0.1f) distance = 0.1f;

    // Calculate sprite screen position
    int screenX = (int)((angle / FOV + 0.5f) * SCREEN_WIDTH);

    // Check if sprite is in front of camera and closer than wall at same column
    if (screenX >= 0 && screenX < SCREEN_WIDTH && depthBuffer[screenX] > distance)
    {

        // Calculate sprite height based on distance with scaling factor
        float spriteHeight = (SCREEN_HEIGHT * TILE_SIZE) / distance * scale;

        // Draw sprite with vertical scaling based on distance with shading
        DrawTexturePro(
            tex,
            (Rectangle){ 0, 0, (float)tex.width, (float)tex.height },
            (Rectangle){ (float)screenX - (spriteHeight / 2.0f),
                         (SCREEN_HEIGHT / 2.0f) - (spriteHeight / 2.0f),
                         spriteHeight, spriteHeight },
            (Vector2){ 0, 0 }, 0.0f, color);
    }
}
