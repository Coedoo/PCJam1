
struct EnemySpawn {
    double spawnTime;
    Vector2 spawnPoint;
    float direction;
    EnemyType enemyType;

    bool spawned;
};

struct Level {
    bool started;
    double startTime;

    double sequenceDuration;

    int spawnsCount;
    EnemySpawn* spawnSequence;
};

void ResetSpawns(Level* level);
void StartLevel(Level* level) {
    level->started = true;
    level->startTime = GetTime();

    ResetSpawns(level);
}

void SpawnSequence(Level level) {
    if(level.started == false) {
        return;
    }

    double currentTime = GetTime() - level.startTime;

    for(int i = 0; i < level.spawnsCount; i++) {
        EnemySpawn* s = level.spawnSequence + i;

        if(currentTime > s->spawnTime && s->spawned == false) {
            s->spawned = true;
            Entity preset = enemyPresets[s->enemyType]; 
            
            printf("Spawn index: %d, [%s] at time: %f\n", i, preset.tag, currentTime);
            
            Vector3 point = {};
            point.x = Lerp(cameraBounds.min.x, cameraBounds.max.x, s->spawnPoint.x);
            point.y = Lerp(cameraBounds.max.x, cameraBounds.min.y, s->spawnPoint.y);

            preset.rotation = s->direction;

            SpawnEntity(preset, point);
            gameState.enemiesCount += 1;

            if(i == level.spawnsCount - 1) {
                gameState.levelCompleted = true;
            }
        }
    }
}


void SimpleSequence(Level* level, int count, float time, float timeInterval, Vector2 spawnPoint, Vector2 pointInverval, float direction, EnemyType enemyType) {

    for(int i = 0; i < count; i++) {
        EnemySpawn* spawn = level->spawnSequence + level->spawnsCount;

        spawn->enemyType = enemyType;
        spawn->spawnTime = time + i * timeInterval;
        spawn->spawnPoint = spawnPoint + (float)i * pointInverval;
        spawn->direction = direction;

        level->spawnsCount += 1;
    }
}

void FillSpawnSequence(Level* level) {
    level->spawnSequence = (EnemySpawn*) malloc(sizeof(EnemySpawn) * 128);

    SimpleSequence(level, 4, 2.0f, 0.4f, {0.15f, 0}, { 0.0f, 0.0f}, 270, Walker);
    SimpleSequence(level, 4, 5.0f, 0.4f, {0.85f, 0}, {0, 0.0f},     270, Walker);

    SimpleSequence(level, 6, 8.0f, 0.2f, {1, 0.25f}, {0, 0.0f},      225, WalkerShooter);
    SimpleSequence(level, 6, 10.0f, 0.2f, {0, 0.25f}, {0, 0.0f},     225 + 90, WalkerShooter);

    SimpleSequence(level, 4, 15.0f, 0.4f, {0.1f, 1}, {0.1f, 0.0f},  90, Walker);
    SimpleSequence(level, 4, 15.0f, 0.4f, {0.9f, 1}, {-0.1f, 0.0f}, 90, Walker);

    SimpleSequence(level, 1, 19.0f, 0.4f, {0.5f, 1}, {-0.1f, 0.0f}, 270, BigOne);

    SimpleSequence(level, 4, 25.0f, 0.4f, {0.2f, 0}, { 0.0f, 0.0f}, 270, Walker);
    SimpleSequence(level, 4, 26.0f, 0.4f, {0.8f, 0}, {0, 0.0f},     270, Walker);
    SimpleSequence(level, 4, 27.0f, 0.4f, {0.4f, 0}, {0, 0.0f},     270, Walker);
    SimpleSequence(level, 4, 28.0f, 0.4f, {0.6f, 0}, {0, 0.0f},     270, Walker);


    SimpleSequence(level, 1, 32.0f, 0.4f, {0.8f, 0}, {-0.1f, 0.0f}, 270, BigOne);

    SimpleSequence(level, 6, 38.0f, 0.4f, {0.5f, 0}, {0, 0.0f}, 265, WalkerShooter);
    SimpleSequence(level, 6, 38.0f, 0.4f, {0.5f, 0}, {0, 0.0f}, 275, WalkerShooter);

    SimpleSequence(level, 6, 43.0f, 0.4f, {0.2f, 0}, {0, 0.0f}, 265, WalkerShooter);
    SimpleSequence(level, 6, 43.0f, 0.4f, {0.2f, 0}, {0, 0.0f}, 275, WalkerShooter);

    SimpleSequence(level, 6, 47.0f, 0.4f, {0.7f, 0}, {0, 0.0f}, 265, WalkerShooter);
    SimpleSequence(level, 6, 47.0f, 0.4f, {0.7f, 0}, {0, 0.0f}, 275, WalkerShooter);


    SimpleSequence(level, 1, 52.0f, 0.1f, {0.2f, 0}, {0.6f, 0.0f}, 260, BigOne);
    SimpleSequence(level, 1, 52.0f, 0.1f, {0.8f, 0}, {0.6f, 0.0f}, 280, BigOne);
}

void ResetSpawns(Level* level) {
    for(int i = 0; i < level->spawnsCount; i++) {
        level->spawnSequence[i].spawned = false;
    }
}

void DrawLevelUI() {
    char* temp = (char*) arena_alloc(&tempArena, 1024);

    sprintf(temp, "Lifes: %d", gameState.currentPlayerLifes);
    DrawText(temp, 0, 0, 40, WHITE);
    sprintf(temp, "Score: %d", gameState.score);
    DrawText(temp, 0, 45, 40, WHITE);
}

void DrawTextCentered(char* text, Vector2 position, float fontSize, float spacing) {
    Vector2 textSize = MeasureTextEx(GetFontDefault(), text, fontSize, spacing) / 2.0f;
    Vector2 pos = {position.x - textSize.x, position.y - textSize.y};

    DrawTextEx(GetFontDefault(), text, pos, fontSize, spacing, WHITE);
}

void DrawGameOverUI() {
    char* gameOverText = "GAME OVER";
    const float fontSize = 60;
    const float spacing = 4;

    Vector2 pos = {windowWidth / 2.0f, windowHeight / 2.0f};
    DrawTextCentered(gameOverText, pos, fontSize, spacing);

    if((float)GetTime() - gameState.stateSwitchTime > 3) {
        char* text = "Press space bar to continue";

        pos = {windowWidth / 2.0f, windowHeight / 2.0f + 40};
        DrawTextCentered(text, pos, 35, spacing);

        if(IsKeyPressed(KEY_SPACE)) {
            GoToTitleScreen();
        }
    }
}

void DrawWinScreen() {
    char* gameOverText = "Frogs Killed!";
    const float fontSize = 60;
    const float spacing = 4;

    Vector2 textSize = MeasureTextEx(GetFontDefault(), gameOverText, fontSize, spacing) / 2.0f;
    Vector2 pos = {windowWidth / 2.0f - textSize.x, windowHeight / 2.0f - textSize.y};

    DrawTextEx(GetFontDefault(), gameOverText, pos, fontSize, spacing, WHITE);

    if((float)GetTime() - gameState.stateSwitchTime > 2) {
        char* text = "Press space bar to continue";

        pos = {windowWidth / 2.0f, windowHeight / 2.0f + 40};
        DrawTextCentered(text, pos, 35, spacing);

        if(IsKeyPressed(KEY_SPACE)) {
            GoToTitleScreen();
        }
    }
}

void PlayerWonControlFunc(Entity* player) {
    float t = (float) GetTime() - player->spawnTime;

    // @NOTE: to be sure...
    player->tint = WHITE;

    if(t < winAnim1Time) {
        float p  = t / winAnim1Time;
        player->position = Vector3Lerp(gameState.playerWonPosition, {0,-2,0}, EaseOutQuart(p));
    }
    else {
        float p = (t - winAnim1Time) / winAnim2Time;
        player->position = Vector3Lerp({0,-2,0}, {0,cameraBounds.max.y + 10,0}, p);
    }
}