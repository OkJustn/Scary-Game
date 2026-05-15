#ifndef MAP_H
#define MAP_H

#include "types.h"

// Returns true if tile (x,y) is impassable (wall or locked door)
bool isWall(int x, int y, int map[MAP_HEIGHT][MAP_WIDTH], bool hasKey);

// Copies level data into GameState and sets spawn positions for all entities
void LoadLevel(int levelNum, Player *p, Monster *m, Key *k,
               GameState *state, int allLevels[NUM_LEVELS][MAP_HEIGHT][MAP_WIDTH]);

#endif // MAP_H
