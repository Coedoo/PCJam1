const int TempArenaSize = 4 * 1024 * 1024;

const int windowWidth = 1600;
const int windowHeight = 900;

////
// Title Screen
///

const Vector2 logoFinalPosition = {windowWidth / 2.0f, windowHeight * 0.75f};
const float titleAnimationTime = 20;

const int terrainResX = 20;
const int terrainResY = 300;

///
/// Player
///

const Vector3 playerShootOffset1 = {0, 0.5f};
const Vector3 playerShootOffset2 = {0.2f, 0.1f};
const Vector3 playerShootOffset3 = {-0.2f, 0.1f};

const int playerDamage = 10;
const float playerSpeed = 7;
const float playerBulletSpeed = 15;

const float spawnAnimationTime = 1.5;
const float invicibilityTime = spawnAnimationTime + 2;

const int playerLifes = 1;

const float playerFireInterval = 0.1f;

const float winAnim1Time = 3;
const float winAnim2Time = 0.5f;

///////
// Enemies
/////

const int walkerHP = 80;
const float walkerMoveSpeed = 3;

const float shooterInitFireTime = 1;
const float shooterFireTime = 3;
const float shooterBulletSpeed = 6;


/// Debug Flags
bool showDebug;
bool showStateWindow;
int controlCamera;
int drawCollisionShapes;

///////////

enum State {
    Title,
    Game,
    GameOver,
    GameWon,
};

struct GameState {
    State state;
    float stateSwitchTime;

    float gameStartTime;

    EntityHandle playerHandle;

    int currentPlayerLifes;
    int score;

    bool levelCompleted;
    int enemiesCount;

    Vector3 playerWonPosition;
};

GameState gameState;