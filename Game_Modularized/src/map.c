#include "map.h"
#include <string.h>

// ─── Global State ─────────────────────────────────────────────────────────────
int map[MAP_HEIGHT][MAP_WIDTH];
bool playerHasKey = false;
float depthBuffer[SCREEN_WIDTH];

// ─── Level Data ───────────────────────────────────────────────────────────────
static int allLevels[NUM_LEVELS][MAP_HEIGHT][MAP_WIDTH] = {
    // Level 1
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1},
        {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    },
    // Level 2
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1},
        {1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1},
        {1, 0, 0, 0, 1, 0, 2, 1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    },
    // Level 3
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 2, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    },
    // Level 4
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
        {1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1},
        {1, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    },
    // Level 5
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 1},
        {1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1},
        {1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1},
        {1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    },
};

// ─── Wall Detection ───────────────────────────────────────────────────────────
bool isWall(int x, int y)
{
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
        return true;
    if (map[y][x] == DOOR_TILE && !playerHasKey)
        return true;
    return (map[y][x] == 1);
}

// ─── Level Loader ─────────────────────────────────────────────────────────────
void LoadLevel(int levelNum, Player *p, Monster *m, Key *k)
{
    int idx = levelNum - 1;

    // Copy the level layout from the predefined levels into the global map
    for (int y = 0; y < MAP_HEIGHT; y++)
        for (int x = 0; x < MAP_WIDTH; x++)
            map[y][x] = allLevels[idx][y][x];

    playerHasKey = false;
    k->active = true;

    // Set initial positions for the player, monster, and key based on the level number
    switch (levelNum)
    {
    case 1:
        p->position = (Vector2){TILE_SIZE * 1.5f, TILE_SIZE * 1.5f};
        m->position = (Vector2){TILE_SIZE * 5.5f, TILE_SIZE * 5.5f};
        k->position = (Vector2){TILE_SIZE * 10.5f, TILE_SIZE * 1.5f};
        break;
    case 2:
        p->position = (Vector2){TILE_SIZE * 1.5f, TILE_SIZE * 10.5f};
        m->position = (Vector2){TILE_SIZE * 1.5f, TILE_SIZE * 1.5f};
        k->position = (Vector2){TILE_SIZE * 10.5f, TILE_SIZE * 1.5f};
        break;
    case 3:
        p->position = (Vector2){TILE_SIZE * 1.5f, TILE_SIZE * 1.5f};
        m->position = (Vector2){TILE_SIZE * 10.5f, TILE_SIZE * 10.5f};
        k->position = (Vector2){TILE_SIZE * 6.5f, TILE_SIZE * 5.5f};
        break;
    case 4:
        p->position = (Vector2){TILE_SIZE * 5.5f, TILE_SIZE * 5.5f};
        m->position = (Vector2){TILE_SIZE * 10.5f, TILE_SIZE * 1.5f};
        k->position = (Vector2){TILE_SIZE * 1.5f, TILE_SIZE * 1.5f};
        break;
    case 5:
        p->position = (Vector2){TILE_SIZE * 3.5f, TILE_SIZE * 10.5f};
        m->position = (Vector2){TILE_SIZE * 1.5f, TILE_SIZE * 1.5f};
        k->position = (Vector2){TILE_SIZE * 8.5f, TILE_SIZE * 6.5f};
        break;
    }
}
