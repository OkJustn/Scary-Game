#include "player.h"
#include "map.h"

void UpdatePlayer(Player *p, GameState *state, float dt)
{
    // Mouse look
    Vector2 mouseDelta = GetMouseDelta();
    p->angle += mouseDelta.x * p->rotSpeed * dt;

    // WASD direction vector
    Vector2 moveVec = { 0, 0 };
    if (IsKeyDown(KEY_W)) moveVec.x += 1;
    if (IsKeyDown(KEY_S)) moveVec.x -= 1;
    if (IsKeyDown(KEY_D)) moveVec.y += 1;
    if (IsKeyDown(KEY_A)) moveVec.y -= 1;

    // If moving, normalize and apply speed
    if (Vector2Length(moveVec) > 0)
    {
        // Normalize
        moveVec = Vector2Normalize(moveVec);
        // WASD's angle + initial player angle 
        float moveAngle = atan2f(moveVec.y, moveVec.x) + p->angle;

        // Calculate next position based on angle and speed
        Vector2 next = {
            p->position.x + cosf(moveAngle) * p->speed * dt,
            p->position.y + sinf(moveAngle) * p->speed * dt,
        };

        // Separate X and Y checking of collision allows wall sliding
        // Check for x
        if (!isWall((int)(next.x / TILE_SIZE),
                    (int)(p->position.y / TILE_SIZE), state->map, p->hasKey))
            p->position.x = next.x;

        // Check for y
        if (!isWall((int)(p->position.x / TILE_SIZE),
                    (int)(next.y / TILE_SIZE), state->map, p->hasKey))
            p->position.y = next.y;
    }
}
