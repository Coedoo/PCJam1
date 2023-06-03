
struct EnemySpawn {
    double spawnTime;
    Vector3 spawnPoint;
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
            
            SpawnEntity(preset, s->spawnPoint);
        }
    }
}

void FillSpawnSequence(Level* level) {

    level->spawnsCount = 4;
    level->spawnSequence = (EnemySpawn*) malloc(sizeof(EnemySpawn) * 4);

    level->spawnSequence[0] = {1,   {-1, 5, 0}, Walker};
    level->spawnSequence[1] = {1.4, { 1, 5, 0}, Walker};
    level->spawnSequence[2] = {1.8, {-2, 5, 0}, Walker};
    level->spawnSequence[3] = {2.4, { 2, 5, 0}, Walker};
}
