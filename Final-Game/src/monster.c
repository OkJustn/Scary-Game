#include "monster.h"
#include "map.h"

void UpdateMonster(Monster *m, Player *p, GameState *state, float dt)
{
    // Direction vector from monster to player (x and y components)
    Vector2 mDir  = Vector2Normalize(Vector2Subtract(p->position, m->position));
    
    // Calculate next position based on direction and speed
    Vector2 mNext = {
        m->position.x + mDir.x * m->speed * dt,
        m->position.y + mDir.y * m->speed * dt,
    };

    // Separate X and Y checking of collision allows wall sliding
    // Check for x
    if (!isWall((int)(mNext.x / TILE_SIZE),
                (int)(m->position.y / TILE_SIZE), state->map, p->hasKey))
        m->position.x = mNext.x;

    // Check for y
    if (!isWall((int)(m->position.x / TILE_SIZE),
                (int)(mNext.y / TILE_SIZE), state->map, p->hasKey))
        m->position.y = mNext.y;
}
