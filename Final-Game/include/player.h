#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"

// Reads mouse/keyboard input and moves the player; handles wall sliding
void UpdatePlayer(Player *p, GameState *state, float dt);

#endif // PLAYER_H
