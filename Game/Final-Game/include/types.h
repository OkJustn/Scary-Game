#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"
#define RAYMATH_STATIC_INLINE
#include "raymath.h"
#include <stdbool.h>
#include <math.h>
#include <string.h>

// Game constants
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

// Game structs
// Player struct
typedef struct Player {
    Vector2 position;
    float   angle;
    float   speed;
    float   rotSpeed; // Mouse sensitivity
    bool    hasKey;
} Player;

// Monster struct
typedef struct Monster {
    Vector2 position;
    float   speed;
    bool    active; // for rendering
} Monster;

// Key struct
typedef struct Key {
    Vector2 position;
    bool    active; // for rendering
} Key;

// Ray info
typedef struct RayHit {
    float distance;
    bool  hitVertical; // for texture shading
    float wallXHit; // x coord of wall hit
    float wallYHit; // y coord of wall hit
    int   tileHit;   // 0=none, 1=wall, 2=door
} RayHit;

// Main game state 
typedef struct GameState {
    int   map[MAP_HEIGHT][MAP_WIDTH];
    float depthBuffer[SCREEN_WIDTH]; // for sprite rendering
    bool  gameOver; // game over flag
    float gameOverTimer; // counter
} GameState;

// Game screen state enum
typedef enum GameScreen {
    MENU,
    GAME,
    PAUSE,
    GAMEOVER,
    INSTRUCT,
} GameScreen;


#endif // TYPES_H
