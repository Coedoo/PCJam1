
struct EnemySpawn {
    double spawnTime;
    Vector2 spawnPoint;
    EnemyType enemyType;
};

struct Level {
    bool started;
    double startTime;

    double sequenceDuration;

    int spawnsCount;
    EnemySpawn* spawnSequence;
};

void StartLevel(Level* level) {
    level->started = true;
    level->startTime = GetTime();
}

void SpawnSequence(Level level) {
    if(level.started == false) {
        return;
    }

    double currentTime = GetTime() - level.startTime;
    double prevTime = currentTime - GetFrameTime();

    for(int i = 0; i < level.spawnsCount; i++) {
        EnemySpawn* s = level.spawnSequence + i;

        if(prevTime < s->spawnTime && currentTime >= s->spawnTime) {
            Entity preset = enemyPresets[s->enemyType]; 
            
            printf("Spawn index: %d, [%s] at time: %f\n", i, preset.tag, currentTime);
            
            Vector3 point = {};
            point.x = Lerp(cameraBounds.min.x, cameraBounds.max.x, s->spawnPoint.x);
            point.y = Lerp(cameraBounds.max.x, cameraBounds.min.y, s->spawnPoint.y);
            SpawnEntity(preset, point);
        }
    }
}


void SimpleSequence(Level* level, int count, float time, float timeInterval, Vector2 spawnPoint, Vector2 pointInverval, EnemyType enemyType) {

    for(int i = 0; i < count; i++) {
        EnemySpawn* spawn = level->spawnSequence + level->spawnsCount;

        spawn->enemyType = enemyType;
        spawn->spawnTime = time + i * timeInterval;
        spawn->spawnPoint = spawnPoint + (float)i * pointInverval;

        level->spawnsCount += 1;
    }
}

void FillSpawnSequence(Level* level) {
    level->spawnSequence = (EnemySpawn*) malloc(sizeof(EnemySpawn) * 16);

    SimpleSequence(level, 4, 1.0f, 0.4f, {0.15f, 0}, { 0.2f, 0.0f}, Walker);
    SimpleSequence(level, 4, 5.0f, 0.4f, {0.85f, 0}, {-0.2f, 0.0f}, Walker);
}


void DrawLevelUI() {
    char* temp = (char*) arena_alloc(&tempArena, 1024);

    sprintf(temp, "Lifes: %d", gameState.currentPlayerLifes);
    DrawText(temp, 0, 0, 40, WHITE);
    sprintf(temp, "Score: %d", gameState.score);
    DrawText(temp, 0, 45, 40, WHITE);
}

void DrawGameOverUI() {
    char* gameOverText = "GAME OVER";
}