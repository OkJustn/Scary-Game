#include "monster.h"
#include "map.h"

void UpdateMonster(Monster *m, Player *p, float dt)
{
    // Skip update if monster is inactive
    Vector2 mDir = Vector2Normalize(Vector2Subtract(p->position, m->position));
    Vector2 mNext = {
        m->position.x + mDir.x * m->speed * dt,
        m->position.y + mDir.y * m->speed * dt,
    };

    // Axis-separated collision (same pattern as player)
    if (!isWall((int)(mNext.x / TILE_SIZE), (int)(m->position.y / TILE_SIZE)))
        m->position.x = mNext.x;
    if (!isWall((int)(m->position.x / TILE_SIZE), (int)(mNext.y / TILE_SIZE)))
        m->position.y = mNext.y;
}
