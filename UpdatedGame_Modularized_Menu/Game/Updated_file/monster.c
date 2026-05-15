#include "monster.h"
#include "map.h"

void UpdateMonster(Monster *m, Player *p, GameState *state, float dt)
{
    Vector2 mDir  = Vector2Normalize(Vector2Subtract(p->position, m->position));
    Vector2 mNext = {
        m->position.x + mDir.x * m->speed * dt,
        m->position.y + mDir.y * m->speed * dt,
    };

    // Axis-separated collision allows wall sliding
    if (!isWall((int)(mNext.x / TILE_SIZE),
                (int)(m->position.y / TILE_SIZE), state->map, p->hasKey))
        m->position.x = mNext.x;

    if (!isWall((int)(m->position.x / TILE_SIZE),
                (int)(mNext.y / TILE_SIZE), state->map, p->hasKey))
        m->position.y = mNext.y;
}
