
#if WEB_BUILD
#include <emscripten/emscripten.h>
#endif

#define _CRT_SECURE_NO_WARNINGS

#include <inttypes.h>
#include <assert.h>

typedef uint32_t u32;
typedef int32_t i32;

struct Str8 {
    char* str;
    uint64_t len;

    char operator[] (int index) {
        assert(index < len && index >= 0);
        return str[index];
    }
};

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "include/raylib.h"
#include "include/rlgl.h"

#define RAYMATH_IMPLEMENTATION
#include "include/raymath.h"
#undef RAYMATH_IMPLEMENTATION

#include "microui/microui.c"
#include "microui/microui_renderer.cpp"

#include "arena.cpp"

Arena tempArena;

Texture2D bulletTexture;
Texture2D blankTexture;

Camera camera;
BoundingBox cameraBounds;

Shader bloomShader;

#include "config.h"

#include "common.cpp"
#include "sprite.cpp"
#include "title_screen.cpp"
#include "entity.cpp"
#include "level.cpp"

void UpdateDrawFrame();

mu_Context muCtx;

Shader terrainShader;
Model terrainModel;

int timeLoc;
int resLoc;
Shader skyboxShader;

TitleScreen titleScreen;
Level level;

bool isInTitleScreen;

RenderTexture2D renderTexture;

void DebugWindow() {
    if(mu_begin_window(&muCtx, "Debug", mu_rect(0, 0, 200, 150))) {
        mu_checkbox(&muCtx, "Camera control", &controlCamera);
        mu_checkbox(&muCtx, "Draw Collision", &drawCollisionShapes);

        char tmp[128];
        Vector2 pos = GetMousePosition();
        sprintf(tmp, "Pos: {%f, %f}", pos.x, pos.y);

        mu_text(&muCtx, tmp);

        mu_end_window(&muCtx);
    }
}

// //// terrain mesh
Mesh CreateTerrainMesh() {
    Mesh mesh = {};
    int vertsCount     = terrainResX * terrainResY;
    int trianglesCount = (terrainResX - 1) * (terrainResY - 1) * 6;

    mesh.vertexCount = vertsCount;
    mesh.triangleCount = (terrainResX - 1) * (terrainResY - 1) * 2;

    mesh.vertices   = (float*) arena_alloc(&tempArena, vertsCount * sizeof(float) * 3);
    mesh.texcoords  = (float*) arena_alloc(&tempArena, vertsCount * sizeof(float) * 2);
    mesh.texcoords2 = (float*) arena_alloc(&tempArena, vertsCount * sizeof(float) * 2);
    mesh.indices    = (unsigned short*) arena_alloc(&tempArena, trianglesCount * sizeof(unsigned short));

    float offset = terrainResX / 2.0f - 0.5f;

    Vector3* verts = (Vector3*)mesh.vertices; 
    Vector2* uvs = (Vector2*)mesh.texcoords;
    Vector2* uvs2 = (Vector2*)mesh.texcoords2;

    for(int y = 0; y < terrainResY; y++) {
        for(int x = 0; x < terrainResX; x++) {
            int idx = x + y * terrainResX;
            verts[idx] = {(float) x - offset, 0, (float) y};
            uvs[idx] = {(float)x / (terrainResX - 1), (float) y / (terrainResY - 1)};
            uvs2[idx] = {(float)(x%2) , (float)(y%2)};
        }
    }

    int index = 0;
    for (int y = 0; y < terrainResY - 1; y++) {
        for (int x = 0; x < terrainResX - 1; x++) {
            int idx = x + y * terrainResX;

            mesh.indices[index++] = idx;
            mesh.indices[index++] = idx + terrainResX + 1;
            mesh.indices[index++] = idx + 1;

            mesh.indices[index++] = idx;
            mesh.indices[index++] = idx + terrainResX;
            mesh.indices[index++] = idx + terrainResX + 1;
        }
    }

    UploadMesh(&mesh, false);

    return mesh;
}

int main()
{
    InitWindow(windowWidth, windowHeight, "Template");

    void* tempArenaBuffer = malloc(TempArenaSize);
    arena_init(&tempArena, tempArenaBuffer, TempArenaSize);
    ////////
    
    InitAudioDevice();
    SetMasterVolume(1);

    ////////

    titleScreen = CreateTitleScreen();
    StartTitleScreenAnim(&titleScreen);
    isInTitleScreen = true;
    ////

    // Setup camera
    camera.position = Vector3{ 0.0f, 0.0f, 3.4f };
    camera.fovy = 90.0f;
    
    camera.target   =  camera.position +  Vector3{0, 0, -1};
    camera.up       = Vector3{ 0.0f, 1.0f, 0.0f };
    camera.projection = CAMERA_PERSPECTIVE;

    Ray botLeft  = GetMouseRay({0, (float)GetScreenHeight()}, camera);
    Ray topRight = GetMouseRay({(float)GetScreenWidth(), 0}, camera);

    RayCollision min = GetRayCollisionQuad(botLeft, {-1000, 1000, 0}, {1000, 1000, 0}, {1000, -1000, 0}, {-1000, -1000, 0});
    RayCollision max = GetRayCollisionQuad(topRight, {-1000, 1000, 0}, {1000, 1000, 0}, {1000, -1000, 0}, {-1000, -1000, 0});

    cameraBounds.min = min.point;
    cameraBounds.max = max.point;
    ///////////
    
    Texture2D theoTexture = LoadTexture("assets/theo_1.png");
    bulletTexture = LoadTexture("assets/theo_1.png");
    blankTexture = LoadTexture("assets/blank.png");

    //////
    CreatePlayerEntity(theoTexture);

    // Entity* blank = CreateEntity();
    // blank->flags = (Render | Collision | HaveHealth);
    // blank->texture = &blankTexture;
    // blank->position.y = 2;
    // blank->scale = {1, 1, 1};
    // blank->collisionType = AABB;
    // blank->collisionSize = {1, 1};
    // blank->maxHP = 100;
    // blank->HP = 100;
    // blank->collisionLayer = ColLay_Enemy | ColLay_PlayerBullet;

    terrainModel = LoadModelFromMesh(CreateTerrainMesh());

    terrainShader = LoadShader("assets/shaders/terrain.vert", "assets/shaders/terrain.frag");
    // timeLoc = GetShaderLocation(terrainShader, "time");
    terrainModel.materials[0].shader = terrainShader;

    skyboxShader = LoadShader(0, "assets/shaders/skybox.frag");
    resLoc = GetShaderLocation(skyboxShader, "resolution");
    timeLoc = GetShaderLocation(skyboxShader, "time");

    bloomShader = LoadShader(0, "assets/shaders/bloom.frag");

    InitEnemyPresets();

    FillSpawnSequence(&level);

    renderTexture = LoadRenderTexture(windowWidth, windowHeight);

    muiInit(&muCtx);

    //////////////////////////
    
    // rlDisableDepthTest();
    // rlDisableDepthMask();

    /////////////////////////

#if WEB_BUILD
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);
    while(WindowShouldClose() == false) {
        UpdateDrawFrame();
    }
#endif

    CloseWindow();
    return 0;
}

void UpdateDrawFrame()
{
    arena_free_all(&tempArena);

    muiProcessInput(&muCtx);
    mu_begin(&muCtx);

    if(showDebug == false) {
        if(IsKeyPressed(KEY_S)) {
            StartLevel(&level);
        }
        /////
        // Level Update
        ////
        SpawnSequence(level);

        /////
        // Entity Update
        ////

        for(int i = 0; i < MAX_ENTITY; i++) {
            Entity* e = entities + i;

            if(e->handle.index == 0) {
                continue;
            }

            if(e->ControlFunction) {
                e->ControlFunction(entities + i);
            }

            u32 f = e->flags;
            if(f & HaveHealth) {
                if(e->HP <= 0) {
                    DestroyEntity(e->handle);
                }
            }
            else if(f & LifeTime) {
                if(e->spawnTime + e->lifeTime <= GetTime()) {
                    DestroyEntity(e->handle);
                }
            }
        }

        // collisions
        // @TODO: I'm not sure if it would be better to put it in main loop
        for(int ai = 0; ai < MAX_ENTITY; ai++) {
            Entity* a = entities + ai;
            if(a->handle.index == 0 || (a->flags & Collision) == 0) {
                continue;
            }

            for(int bi = ai + 1; bi < MAX_ENTITY; bi++) {
                Entity* b = entities + bi;
                if(a->handle.index == 0 || (b->flags & Collision) == 0) {
                    continue;
                }

                if((a->collisionLayer & b->collisionLayer) == 0) {
                    continue;
                }

                assert(a->collisionType != None && b->collisionType != None);

                bool collision = false;
                // @TODO: implement other collision types
                if(a->collisionType == AABB && b->collisionType == AABB) {
                    float aLeft  = a->position.x - a->collisionSize.x / 2;
                    float aRight = a->position.x + a->collisionSize.x / 2;
                    float aTop   = a->position.y + a->collisionSize.y / 2;
                    float aBot   = a->position.y - a->collisionSize.y / 2;

                    float bLeft  = b->position.x - b->collisionSize.x / 2;
                    float bRight = b->position.x + b->collisionSize.x / 2;
                    float bTop   = b->position.y + b->collisionSize.y / 2;
                    float bBot   = b->position.y - b->collisionSize.y / 2;

                    collision = aLeft <= bRight &&
                                 aRight >= bLeft &&
                                 aBot <= bTop    &&
                                 aTop >= bBot;

                }
                else if(a->collisionType == Circle && b->collisionType == Circle) {
                    float distSqr = Vector3DistanceSqr(a->position, b->position);
                    float rad = a->collisionSize.x + b->collisionSize.x;
                    collision = distSqr < rad * rad;
                }
                else if((a->collisionType == AABB && b->collisionType == Circle) || 
                        (a->collisionType == Circle && b->collisionType == AABB)) {
                    Entity* aabb   = a->collisionType == AABB ? a : b;
                    Entity* circle = a->collisionType == Circle ? a : b;

                    float minX = aabb->position.x - aabb->collisionSize.x / 2;
                    float maxX = aabb->position.x + aabb->collisionSize.x / 2;
                    float maxY = aabb->position.y + aabb->collisionSize.y / 2;
                    float minY = aabb->position.y - aabb->collisionSize.y / 2;

                    float x = max(minX, min(circle->position.x, maxX));
                    float y = max(minY, min(circle->position.y, maxY));

                    float dist = (x - circle->position.x) * (x - circle->position.x) +
                                 (y - circle->position.y) * (y - circle->position.y);

                    collision = dist < circle->collisionSize.x * circle->collisionSize.x;
                } 

                if(collision) {
                    printf("Collision! [%s] and [%s]\n", a->tag, b->tag);
                    // showDebug = true;
                    //@TODO: Handle collision function
                    if(a->collisionflags & ColFlag_Damage) {
                        b->HP -= a->damage;
                    }
                    if(a->collisionflags & ColFlag_DestroyAfterHit) {
                        DestroyEntity(a->handle);
                    }


                    if(b->collisionflags & ColFlag_Damage) {
                        a->HP -= b->damage;
                    }
                    if(b->collisionflags & ColFlag_DestroyAfterHit) {
                        DestroyEntity(b->handle);
                    }
                }
            }
        }
    }

    if(controlCamera) {
        UpdateCamera(&camera, CAMERA_FREE);
    }

    if(IsKeyPressed(KEY_Y)) {
        showDebug = !showDebug;
    }
    if(showDebug) {
        DebugWindow();
    }

    mu_end(&muCtx);

    // BeginDrawing();
    BeginTextureMode(renderTexture);
    ClearBackground({219, 216, 225, 0});

    DrawFPS(0, 0);

    Vector2 res = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    SetShaderValue(skyboxShader, resLoc, &res, RL_SHADER_UNIFORM_VEC2);

    BeginShaderMode(skyboxShader);
    rlBegin(RL_QUADS);
        rlTexCoord2f(1, 1);
        rlVertex2f(res.x, 0);


        rlTexCoord2f(0, 1);
        rlVertex2f(0, 0);

        rlTexCoord2f(0, 0);
        rlVertex2f(0, res.y);


        rlTexCoord2f(1, 0);
        rlVertex2f(res.x, res.y);

    rlEnd();
    EndShaderMode();


    BeginMode3D(camera);
    {
        // DrawGrid(15, 15);
        // DrawCube({0,0,0}, 1, 1, 1, RED);

        float t = (float) GetTime();
        SetShaderValue(skyboxShader, timeLoc, &t, RL_SHADER_UNIFORM_FLOAT);

        for(int i = 0; i < 5; i++) {
            DrawModel(terrainModel, {0, -15, -terrainResY*8*(i+1) + (float)GetTime() * 9}, 8.0f, WHITE);
        }
        /// Render Entities
        for(int i = 0; i < MAX_ENTITY; i++) {
            Entity* e = entities + i;
            if(e->flags & Render) {
                DrawBillboardRec(camera, e->sprite.texture, e->sprite.textureRect, e->position, {e->size, e->size}, WHITE);
            }
        }

        if(drawCollisionShapes) {
            DrawBoundingBox(cameraBounds, GREEN);

            for(int i = 0; i < MAX_ENTITY; i++) {
                Entity* e = entities + i;
                if(e->flags & Collision) {
                    if(e->collisionType == AABB) {
                        DrawCubeWires(e->position, e->collisionSize.x, e->collisionSize.y, 0.0f, GREEN);
                    }
                    else if(e->collisionType == Circle) {
                        DrawCircle3D(e->position, e->collisionSize.x, {0, 0, 1}, 0, GREEN);
                    }
                }
            }
        }
    }
    EndMode3D();
    EndTextureMode();

    BeginDrawing(); 
    // BeginShaderMode(bloomShader);
    
        DrawTextureRec(renderTexture.texture, { 0, 0, (float)renderTexture.texture.width, (float)-renderTexture.texture.height }, { 0, 0 }, WHITE);
    // EndShaderMode();

        UpdateAndDrawTitleScreen(&titleScreen);
        muiRender(&muCtx);
    EndDrawing();


    for(int i = 0; i < MAX_ENTITY; i++) {
        Entity* e = entities + i;
        if(e->toDestroy) {
            u32 genTmp = e->handle.generation;
            memset(e, 0, sizeof(Entity));
            e->handle.generation = genTmp;
        }
    }
}
