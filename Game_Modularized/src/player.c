#include "player.h"
#include "map.h"

void UpdatePlayer(Player *p, float dt)
{
    // Mouse look
    Vector2 mouseDelta = GetMouseDelta();
    p->angle += mouseDelta.x * p->rotSpeed * dt;

    // WASD movement
    Vector2 moveVec = {0, 0};
    if (IsKeyDown(KEY_W))
        moveVec.x += 1;
    if (IsKeyDown(KEY_S))
        moveVec.x -= 1;
    if (IsKeyDown(KEY_D))
        moveVec.y += 1;
    if (IsKeyDown(KEY_A))
        moveVec.y -= 1;

    // Normalize movement vector to prevent faster diagonal movement
    if (Vector2Length(moveVec) > 0)
    {
        moveVec = Vector2Normalize(moveVec);
        float moveAngle = atan2f(moveVec.y, moveVec.x) + p->angle;

        // Calculate the next position based on the movement angle and speed
        Vector2 next = {
            p->position.x + cosf(moveAngle) * p->speed * dt,
            p->position.y + sinf(moveAngle) * p->speed * dt,
        };

        // Axis-separated collision
        if (!isWall((int)(next.x / TILE_SIZE), (int)(p->position.y / TILE_SIZE)))
            p->position.x = next.x;
        if (!isWall((int)(p->position.x / TILE_SIZE), (int)(next.y / TILE_SIZE)))
            p->position.y = next.y;
    }
}
