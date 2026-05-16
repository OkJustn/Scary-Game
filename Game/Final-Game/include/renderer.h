#ifndef RENDERER_H
#define RENDERER_H

#include "types.h"

// Casts a single ray from start at angle; returns hit information
RayHit CastRay(Vector2 start, float angle,
               int map[MAP_HEIGHT][MAP_WIDTH], bool hasKey);

// Draws all wall/door columns for the current frame and fills depthBuffer
void DrawWalls(Player player, GameState *state,
               Texture2D wallTex, Texture2D doorTex, Texture2D portalTex);

// Projects a world-space sprite onto the screen with depth-buffer occlusion
void DrawSprite3D(Vector2 pos, Player player, Texture2D tex,
                  float scale, Color color, float depthBuffer[SCREEN_WIDTH]);

#endif // RENDERER_H
