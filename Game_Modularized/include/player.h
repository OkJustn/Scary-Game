#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"

// Reads input and moves the player for one frame (dt = delta-time in seconds)
void UpdatePlayer(Player *p, float dt);

#endif // PLAYER_H
