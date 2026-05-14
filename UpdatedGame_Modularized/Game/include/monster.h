#ifndef MONSTER_H
#define MONSTER_H

#include "types.h"

// Moves the monster toward the player each frame
void UpdateMonster(Monster *m, Player *p, GameState *state, float dt);

#endif // MONSTER_H
