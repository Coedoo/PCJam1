struct EntityHandle {
    u32 generation;
    u32 index;
};

enum EntityFlag {
    Render = (1 << 0),
    Collision = (1 << 1)
};

struct Entity {
    EntityHandle handle;
    bool toDestroy;

    void (*ControlFunction)(Entity* entity);

    EntityFlag flags;

    // transform
    Vector3 position;
    float rotation;
    Vector3 scale;

    // rendering
    Texture* texture;
};


#define MAX_ENTITY 256
int entityCount;
Entity entities[MAX_ENTITY];

EntityHandle CreateEntityHandle() {
    assert(entityCount < MAX_ENTITY);

    for(int i = 1; i < MAX_ENTITY; i++) {
        if(entities[i].handle.index == 0) {
            entities[i].handle.index = i;
            entities[i].handle.generation += 1;

            entityCount += 1;

            return entities[i].handle;
        }
    }

    return {};
}

/////
// PLAYER
/////

void PlayerControlFunc(Entity* player) {
    Vector2 input = GetMovementInput();

    Vector2 move = input * playerSpeed * GetFrameTime();
    player->position.x += move.x;
    player->position.y += move.y;

    if(IsKeyPressed(KEY_SPACE)) {
        
    }
}

EntityHandle CreatePlayerEntity(Texture2D* texture) {
    EntityHandle handle = CreateEntityHandle();
    assert(handle.index != 0);

    Entity* player = &(entities[handle.index]);

    player->handle = handle;
    player->flags = (EntityFlag)(Render | Collision);
    player->scale = {1, 1, 1};
    player->texture = texture;

    player->ControlFunction = PlayerControlFunc;

    return handle;
}
