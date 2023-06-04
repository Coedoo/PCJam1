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
    SpriteAnim = (1 << 5),
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

u32 collisionsMasks[] = {
    ColLay_PlayerBullet | ColLay_Enemy,
    ColLay_EnemyBullet | ColLay_Player,
    ColLay_Player | ColLay_Enemy,
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
    Sprite sprite;

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

    entities[handle.index].spawnTime = (float) GetTime();

    return entities + handle.index;
}

EntityHandle SpawnEntity(Entity preset, Vector3 position) {
    EntityHandle h = CreateEntityHandle();
    assert(h.index != 0);

    preset.handle = h;
    preset.position = position;
    preset.spawnTime = (float) GetTime();

    entities[h.index] = preset;

    return h;
}

void DestroyEntity(EntityHandle handle) {
    entities[handle.index].toDestroy = true; 
    printf("Destroing entity: [%s]\n", entities[handle.index].tag);
}

Vector2 GetMovementInput() {
    Vector2 input = {};

    if(IsKeyDown(KEY_LEFT))       input.x = -1;
    else if(IsKeyDown(KEY_RIGHT)) input.x =  1;

    if(IsKeyDown(KEY_UP))        input.y =  1;
    else if(IsKeyDown(KEY_DOWN)) input.y = -1;

    return input;
}


///
BoundingBox GetEntityBounds(Entity* entity) {
    BoundingBox ret = {};

    ret.min.x = entity->position.x - entity->collisionSize.x / 2.0f;
    ret.max.x = entity->position.x + entity->collisionSize.x / 2.0f;
    ret.min.y = entity->position.y - entity->collisionSize.y / 2.0f;
    ret.max.y = entity->position.y + entity->collisionSize.y / 2.0f;

    return ret;
}

/////
// PLAYER
/////

void CreatePlayerBullet(Vector3 position);

void PlayerControlFunc(Entity* player) {
    Vector2 input = GetMovementInput();

    Vector2 move = input * playerSpeed * GetFrameTime();
    player->position.x += move.x;
    player->position.y += move.y;

    player->position.x = Clamp(player->position.x, cameraBounds.min.x, cameraBounds.max.x);
    player->position.y = Clamp(player->position.y, cameraBounds.min.y, cameraBounds.max.y);

    if(IsKeyDown(KEY_SPACE)) {
        CreatePlayerBullet(player->position);
    }
}

EntityHandle CreatePlayerEntity() {
    Entity* player = CreateEntity();

    player->tag = "Player";

    player->flags = (EntityFlag)(Render | Collision | HaveHealth | SpriteAnim);
    player->size = 1;
    player->sprite = CreateSprite(atlas, {64, 0, 3 * 32, 2 * 32});
    player->sprite.animationFrames = 2;
    player->sprite.frameTime = 0.2f;
    player->sprite.animDir = Vertical;

    player->collisionType = AABB;
    player->collisionSize = {1, 1};
    player->collisionLayer = ColLay_Player;

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
    bullet->sprite = CreateSprite(atlas, {0, 0, 32, 32});

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
        e.sprite = CreateSprite(blankTexture);
        e.size = 0.5f;
        e.collisionType = AABB;
        e.collisionSize = {0.5f, 0.5f};
        e.HP = walkerHP;
        e.collisionLayer = ColLay_Enemy;

        e.ControlFunction = WalkerControlFunc;

        enemyPresets[Walker] = e;
    }
}