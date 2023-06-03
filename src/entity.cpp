struct EntityHandle {
    u32 generation;
    u32 index;
};

enum EntityFlag {
    Render = (1 << 0),
    Collision = (1 << 1),
    HaveHealth = (1 << 2),
    Damaging = (1 << 3),
    LifeTime = (1 << 4),
};

/////////////

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

enum CollisionLayers {
    ColLay_None = 0,
    ColLay_Player = (1 << 0),
    ColLay_Enemy  = (1 << 1),
    ColLay_PlayerBullet  = (1 << 2),
    ColLay_EnemyBullet  = (1 << 3),
};

/////////////

struct Entity {
    EntityHandle handle;
    bool toDestroy;

    float spawnTime;
    float lifeTime;

    char* tag;

    void (*ControlFunction)(Entity* entity);

    u32 flags;

    // transform
    Vector3 position;
    float rotation;
    float size;

    // rendering
    Texture* texture;

    //
    int maxHP;
    int HP;

    // collision
    CollisionType collisionType;
    u32 collisionflags;
    Vector2 collisionSize;
    u32 collisionLayer;

    //
    int damage;
};


#define MAX_ENTITY 1024
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

    entities[handle.index].spawnTime = GetTime();

    return entities + handle.index;
}

EntityHandle SpawnEntity(Entity preset, Vector3 position) {
    EntityHandle h = CreateEntityHandle();
    assert(h.index != 0);

    preset.handle = h;
    preset.position = position;
    preset.spawnTime = GetTime();

    entities[h.index] = preset;

    return h;
}

void DestroyEntity(EntityHandle handle) {
    entities[handle.index].toDestroy = true; 
    printf("Destroing entity: [%s]\n", entities[handle.index].tag);
}

void CreatePlayerBullet(Vector3 position);

/////
// PLAYER
/////

void PlayerControlFunc(Entity* player) {
    Vector2 input = GetMovementInput();

    Vector2 move = input * playerSpeed * GetFrameTime();
    player->position.x += move.x;
    player->position.y += move.y;

    if(IsKeyDown(KEY_SPACE)) {
        CreatePlayerBullet(player->position);
    }
}

EntityHandle CreatePlayerEntity(Texture2D* texture) {
    Entity* player = CreateEntity();

    player->tag = "Player";

    player->flags = (EntityFlag)(Render | Collision | HaveHealth);
    player->size = 1;
    player->texture = texture;

    player->collisionType = AABB;
    player->collisionSize = {1, 1};

    player->maxHP = 100;
    player->HP = 100;

    player->ControlFunction = PlayerControlFunc;

    return player->handle;
}

//// 
// Bullets
///

void BulletControlFunc(Entity* bullet) {
    bullet->position.y += playerBulletSpeed * GetFrameTime();

    if(bullet->position.y >= cameraBounds.max.y) {
        DestroyEntity(bullet->handle);
    }
}

void CreatePlayerBullet(Vector3 position) {
    Entity* bullet = CreateEntity();

    bullet->tag = "Bullet";

    bullet->position = position;
    bullet->flags = (EntityFlag)(Render | Collision | Damaging | LifeTime);
    bullet->size = 0.2f;
    bullet->texture = &bulletTexture;

    bullet->lifeTime = 2;

    bullet->collisionType = CollisionType::Circle;
    bullet->collisionSize.x = 0.1f;

    bullet->collisionflags = (CollisionFlag)(ColFlag_DestroyAfterHit | ColFlag_Damage);
    bullet->collisionLayer = ColLay_PlayerBullet;

    bullet->damage = playerDamage;

    bullet->ControlFunction = BulletControlFunc;
}

/////
// Enemies
/////
enum EnemyType {
    Walker,
    Count,
};

Entity enemyPresets[EnemyType::Count];

void WalkerControlFunc(Entity* entity) {
    entity->position.y -= GetFrameTime() * walkerMoveSpeed;
}


void InitEnemyPresets() {
    {
        Entity e = {};

        e.tag = "Walker";
        e.flags = (Render | Collision | HaveHealth);
        e.texture = &blankTexture;
        e.size = 1;
        e.collisionType = AABB;
        e.collisionSize = {1, 1};
        e.HP = walkerHP;
        e.collisionLayer = ColLay_Enemy | ColLay_PlayerBullet;

        e.ControlFunction = WalkerControlFunc;

        enemyPresets[Walker] = e;
    }
}