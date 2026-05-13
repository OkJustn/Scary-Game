#ifndef MONSTER_H
#define MONSTER_H

#include "types.h"

// Moves the monster toward the player for one frame
void UpdateMonster(Monster *m, Player *p, float dt);

#endif // MONSTER_H
