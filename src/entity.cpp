struct EntityHandle {
    u32 generation;
    u32 index;
};

enum EntityFlag {
    Render = (1 << 0),
    Collision = (1 << 1),
    HaveHealth = (1 << 2),
    Damageable = (1 << 3)
};

enum CollisionType {
    None,
    AABB,
    Circle
};

enum CollisionFlag {
    ColFlag_None = 0,
    ColFlag_DestroyAfterHit = (1 << 0),
    ColFlag_Damage = (1 << 1)
};

struct Entity {
    EntityHandle handle;
    bool toDestroy;

    void (*ControlFunction)(Entity* entity);

    u32 flags;

    // transform
    Vector3 position;
    float rotation;
    Vector3 scale;

    // rendering
    Texture* texture;

    //
    int maxHP;
    int currentHP;

    // collision
    CollisionType collisionType;
    u32 collisionflags;
    Vector2 collisionSize;
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

Entity* CreateEntity() {
    EntityHandle handle = CreateEntityHandle();
    assert(handle.index != 0);

    return entities + handle.index;
}

void CreateBullet(Vector3 position);

/////
// PLAYER
/////

void PlayerControlFunc(Entity* player) {
    Vector2 input = GetMovementInput();

    Vector2 move = input * playerSpeed * GetFrameTime();
    player->position.x += move.x;
    player->position.y += move.y;

    if(IsKeyDown(KEY_SPACE)) {
        CreateBullet(player->position);
    }
}

EntityHandle CreatePlayerEntity(Texture2D* texture) {
    Entity* player = CreateEntity();

    player->flags = (EntityFlag)(Render | Collision | HaveHealth);
    player->scale = {1, 1, 1};
    player->texture = texture;

    player->collisionType = AABB;
    player->collisionSize = {1, 1};

    player->ControlFunction = PlayerControlFunc;

    return player->handle;
}

//// 
// Bullets
///

void BulletControlFunc(Entity* bullet) {
    bullet->position.y += playerBulletSpeed * GetFrameTime();
}

void CreateBullet(Vector3 position) {
    Entity* bullet = CreateEntity();

    bullet->position = position;
    bullet->flags = (EntityFlag)(EntityFlag::Render | EntityFlag::Collision);
    bullet->scale = {0.2f, 0, 0};
    bullet->texture = &bulletTexture;

    bullet->collisionType = CollisionType::Circle;
    bullet->collisionSize.x = 0.1f;

    bullet->collisionflags = (CollisionFlag)(ColFlag_DestroyAfterHit | ColFlag_Damage);

    bullet->ControlFunction = BulletControlFunc;
}