#include "map.h"

// Level Data
int allLevelsData[NUM_LEVELS][MAP_HEIGHT][MAP_WIDTH] = {
    // Level 1
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1},
        {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    },
    // Level 2
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1},
        {1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1},
        {1, 0, 0, 0, 1, 0, 2, 1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    },
    // Level 3
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 2, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    },
    // Level 4
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
        {1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1},
        {1, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    },
    // Level 5
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 1},
        {1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1},
        {1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1},
        {1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    },
};

// Wall Detection
bool isWall(int x, int y, int map[MAP_HEIGHT][MAP_WIDTH], bool hasKey)
{
    // Check out of bounds
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return true;
    
    // A door is a wall without the key
    if (map[y][x] == DOOR_TILE && !hasKey) return true;

    // Return true if cell is a wall
    return (map[y][x] == 1);
}

// Level Loader
void LoadLevel(int levelNum, Player *p, Monster *m, Key *k,
               GameState *state)
{
    // 0 based index
    int lvl = levelNum - 1;

    // Copy level data into game state
    memcpy(state->map, allLevelsData[lvl], sizeof(state->map));

    p->hasKey = false;
    k->active = true;

    // Set positions of objects each level
    switch (levelNum)
    {
        case 1:                      // Scale  * decimal coordinate
            p->position = (Vector2){ TILE_SIZE * 1.5f,  TILE_SIZE * 1.5f  }; // Player
            m->position = (Vector2){ TILE_SIZE * 1.5f,  TILE_SIZE * 9.5f  }; // Monster
            k->position = (Vector2){ TILE_SIZE * 10.5f, TILE_SIZE * 1.5f  }; // Key
            break;
        case 2:
            p->position = (Vector2){ TILE_SIZE * 1.5f,  TILE_SIZE * 10.5f };
            m->position = (Vector2){ TILE_SIZE * 1.5f,  TILE_SIZE * 1.5f  };
            k->position = (Vector2){ TILE_SIZE * 10.5f, TILE_SIZE * 1.5f  };
            break;
        case 3:
            p->position = (Vector2){ TILE_SIZE * 1.5f,  TILE_SIZE * 1.5f  };
            m->position = (Vector2){ TILE_SIZE * 10.5f, TILE_SIZE * 10.5f };
            k->position = (Vector2){ TILE_SIZE * 6.5f,  TILE_SIZE * 5.5f  };
            break;
        case 4:
            p->position = (Vector2){ TILE_SIZE * 5.5f,  TILE_SIZE * 5.5f  };
            m->position = (Vector2){ TILE_SIZE * 10.5f, TILE_SIZE * 1.5f  };
            k->position = (Vector2){ TILE_SIZE * 1.5f,  TILE_SIZE * 1.5f  };
            break;
        case 5:
            p->position = (Vector2){ TILE_SIZE * 3.5f,  TILE_SIZE * 10.5f };
            m->position = (Vector2){ TILE_SIZE * 1.5f,  TILE_SIZE * 1.5f  };
            k->position = (Vector2){ TILE_SIZE * 8.5f,  TILE_SIZE * 6.5f  };
            break;
    }
}
