
enum EntityFlag {
    Render = (1 << 0),
    Collision = (1 << 1),
    HaveHealth = (1 << 2),
    Damaging = (1 << 3),
    LifeTime = (1 << 4),
    SpriteAnim = (1 << 5),
    DestroyOutsideCamera = (1 << 6),
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

enum DestroyReason {
    LowHP,
    AfterCollision,
    LifeTimeEnded,
    OutsideCamera,
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
    Color tint;

    //
    int HP;
    void (*DestroyCallback)(Entity* entity, DestroyReason reason);

    // collision
    CollisionType collisionType;
    u32 collisionflags;
    Vector2 collisionSize;
    u32 collisionLayer;

    //
    int damage;
    float fireTimer;

    ///
    bool isInsideCamera;
};


#define MAX_ENTITY 1024
int entityCount;
Entity entities[MAX_ENTITY];

bool IsValidHandle(EntityHandle handle) {
    if(handle.index == 0) {
        return false;
    }

    EntityHandle stored = entities[handle.index].handle;

    return stored.index != 0 && stored.generation == handle.generation;
}

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
    entities[handle.index].tint = WHITE;

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

void DestroyEntity(EntityHandle handle, DestroyReason reason) {
    assert(IsValidHandle(handle));

    Entity* e = entities + handle.index;
    if(e->DestroyCallback) {
        e->DestroyCallback(e, reason);
    }

    entities[handle.index].toDestroy = true;
    printf("Destroing entity: [%s]\n", entities[handle.index].tag);
}

Entity* GetEntityPtr(EntityHandle handle) {
    if(IsValidHandle(handle) == false) {
        return nullptr;
    }

    return &entities[handle.index];
}

/////

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
    // Spawn routine
    float liveTime = (float) GetTime() - player->spawnTime;


    if(liveTime > invicibilityTime) {
        player->flags |= Collision;
        player->tint = WHITE;
    }
    else {
        player->tint = ColorLerp(WHITE, SKYBLUE, sinf(liveTime * invicibilityTime) * 0.5f + 0.5f);
    }
    
    if(liveTime < spawnAnimationTime) {
        float p = liveTime / spawnAnimationTime;
        player->position = Vector3Lerp({0, cameraBounds.min.y}, Vector3{0, -2.5f, 0}, p);
        return;
    }

    Vector2 input = GetMovementInput();

    Vector2 move = input * playerSpeed * GetFrameTime();
    player->position.x += move.x;
    player->position.y += move.y;

    player->position.x = Clamp(player->position.x, cameraBounds.min.x, cameraBounds.max.x);
    player->position.y = Clamp(player->position.y, cameraBounds.min.y, cameraBounds.max.y);

    player->fireTimer -= GetFrameTime();
    if(IsKeyDown(KEY_SPACE)) {
        if(player->fireTimer < 0) {

            player->fireTimer = playerFireInterval;
            CreatePlayerBullet(player->position);
            
            PlaySound(audioLib[Shot1]);
        }
    }
}

void PlayerDestroyCallback(Entity* player, DestroyReason reason) {
    gameState.currentPlayerLifes -= 1;
    PlaySound(audioLib[Scream0]);

    if(gameState.currentPlayerLifes < 0) {
        GoToGameOver();
    }
}

EntityHandle CreatePlayerEntity() {
    Entity* player = CreateEntity();

    player->tag = "Player";

    player->position = {0, cameraBounds.min.y};

    player->flags = (EntityFlag)(Render | HaveHealth | SpriteAnim);
    player->size = 1;
    player->sprite = CreateSprite(atlas, {64, 0, 3 * 32, 2 * 32});
    player->sprite.animationFrames = 2;
    player->sprite.frameTime = 0.2f;
    player->sprite.animDir = Vertical;

    player->collisionType = Circle;
    player->collisionSize = {0.1f};
    player->collisionLayer = ColLay_Player;

    player->HP = 1;

    player->ControlFunction = PlayerControlFunc;
    player->DestroyCallback = PlayerDestroyCallback;

    return player->handle;
}

//// 
// Bullets
///

void BulletControlFunc(Entity* bullet) {
    bullet->position.y += playerBulletSpeed * GetFrameTime();
}

void CreatePlayerBullet(Vector3 position) {
    Entity* bullet = CreateEntity();

    bullet->tag = "Bullet";

    bullet->position = position;
    bullet->flags = (EntityFlag)(Render | Collision | Damaging | DestroyOutsideCamera);
    bullet->size = 0.2f;
    bullet->sprite = CreateSprite(atlas, {0, 0, 32, 32});

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

void EnemyDestroyCallback(Entity* entity, DestroyReason reason) {
    gameState.enemiesCount -= 1;

    if(reason == LowHP) {
        PlaySound(audioLib[Dead1]);
    }

    if(gameState.levelCompleted && gameState.enemiesCount == 0) {
        // printf("\n FINISHEEEDDD \n");
        GoToGameWon();
    }
}

void WalkerControlFunc(Entity* entity) {
    entity->position.y -= GetFrameTime() * walkerMoveSpeed;
}

void InitEnemyPresets() {
    {
        Entity e = {};

        e.tag = "Walker";
        e.flags = (Render | Collision | HaveHealth | DestroyOutsideCamera);
        e.sprite = CreateSprite(atlas, {0, 32, 32, 32});
        e.size = 0.5f;
        e.collisionType = AABB;
        e.collisionSize = {0.5f, 0.5f};
        e.HP = walkerHP;
        e.collisionLayer = ColLay_Enemy;
        e.collisionflags = ColFlag_Damage;
        e.damage = 1;
        e.tint = WHITE;

        e.ControlFunction = WalkerControlFunc;
        e.DestroyCallback = EnemyDestroyCallback;

        enemyPresets[Walker] = e;
    }
}