const int TempArenaSize = 4 * 1024 * 1024;

const int windowWidth = 1700;
const int windowHeight = 900;

////
// Title Screen
///

const Vector2 logoFinalPosition = {windowWidth / 2.0f, windowHeight * 0.75f};
const float titleAnimationTime = 20;

const int terrainResX = 20;
const int terrainResY = 150;

///
/// Player
///

const int playerDamage = 10;
const float playerSpeed = 8;
const float playerBulletSpeed = 15;

///////
// Enemies
/////

const int walkerHP = 80;
const float walkerMoveSpeed = 3;


/// Debug Flags
bool showDebug;
int controlCamera;
int drawCollisionShapes;

