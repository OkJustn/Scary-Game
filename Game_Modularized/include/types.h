#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <math.h>

// ─── Screen & Map Constants ───────────────────────────────────────────────────
#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720
#define MAP_WIDTH     12
#define MAP_HEIGHT    12
#define TILE_SIZE     64.0f
#define FOV           (60.0f * DEG2RAD)
#define NUM_RAYS      SCREEN_WIDTH
#define MAX_DEPTH     5.0f
#define DOOR_TILE     2
#define NUM_LEVELS    5

// ─── Structs ──────────────────────────────────────────────────────────────────
typedef struct Player {
    Vector2 position;
    float   angle;
    float   speed;
    float   rotSpeed;
} Player;

typedef struct Monster {
    Vector2 position;
    float   speed;
    bool    active;
} Monster;

typedef struct Key {
    Vector2 position;
    bool    active;
} Key;

typedef struct RayHit {
    float distance;
    bool  hitVertical;
    float wallXHit;
    float wallYHit;
    int   tileHit;
} RayHit;

// ─── Global State (defined in map.c) ─────────────────────────────────────────
extern int  map[MAP_HEIGHT][MAP_WIDTH];
extern bool playerHasKey;
extern float depthBuffer[SCREEN_WIDTH];

#endif // TYPES_H
