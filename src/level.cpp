
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
            gameState.enemiesCount += 1;

            if(i == level.spawnsCount - 1) {
                gameState.levelCompleted = true;
            }
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
    const float fontSize = 60;
    const float spacing = 4;

    Vector2 textSize = MeasureTextEx(GetFontDefault(), gameOverText, fontSize, spacing) / 2.0f;
    Vector2 pos = {windowWidth / 2.0f - textSize.x, windowHeight / 2.0f - textSize.y};

    DrawTextEx(GetFontDefault(), gameOverText, pos, fontSize, spacing, WHITE);
}

void DrawWinScreen() {
    char* gameOverText = "GAME WON";
    const float fontSize = 60;
    const float spacing = 4;

    Vector2 textSize = MeasureTextEx(GetFontDefault(), gameOverText, fontSize, spacing) / 2.0f;
    Vector2 pos = {windowWidth / 2.0f - textSize.x, windowHeight / 2.0f - textSize.y};

    DrawTextEx(GetFontDefault(), gameOverText, pos, fontSize, spacing, WHITE);
}

void PlayerWonControlFunc(Entity* player) {
    float t = (float) GetTime() - player->spawnTime;

    if(t < winAnim1Time) {
        float p  = t / winAnim1Time;
        player->position = Vector3Lerp(gameState.playerWonPosition, {0,-2,0}, EaseOutQuart(p));
    }
    else {
        float p = (t - winAnim1Time) / winAnim2Time;
        player->position = Vector3Lerp({0,-2,0}, {0,cameraBounds.max.y + 10,0}, p);
    }
}