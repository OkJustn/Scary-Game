#ifndef RENDERER_H
#define RENDERER_H

#include "types.h"

// Casts a single ray from 'start' at 'angle'; returns hit information
RayHit CastRay(Vector2 start, float angle);

// Draws all rays (walls/doors) for the current frame
void DrawWalls(Player player, Texture2D wallTex, Texture2D doorTex, Texture2D portalTex);

// Projects a world-space sprite onto the screen with depth-buffer testing
void DrawSprite3D(Vector2 pos, Player player, Texture2D tex, float scale, Color color);

#endif // RENDERER_H
