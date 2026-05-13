#ifndef MAP_H
#define MAP_H

#include "types.h"

// Returns true if the given tile coordinate is a wall (or impassable door)
bool isWall(int x, int y);

// Copies level data into the active map and resets player/monster/key state
void LoadLevel(int levelNum, Player *p, Monster *m, Key *k);

#endif // MAP_H
