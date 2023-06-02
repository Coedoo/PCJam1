
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

Texture2D bulletTexture;
Texture2D blankTexture;

#include "config.h"

#include "common.cpp"
#include "entity.cpp"

void UpdateDrawFrame();

Camera camera;
mu_Context muCtx;

template<typename T>
T max(T& a, T& b) {
    return a > b ? a : b;
}

template<typename T>
T min(T& a, T& b) {
    return a < b ? a : b;
}

void DebugWindow() {
    if(mu_begin_window(&muCtx, "Debug", mu_rect(0, 0, 200, 150))) {
        mu_checkbox(&muCtx, "Camera control", &controlCamera);
        mu_end_window(&muCtx);
    }
}


int main()
{
    InitWindow(1700, 900, "Template");

    // Setup camera
    camera.position = Vector3{ 0.0f, 1.0f, 4.0f };
    camera.target =  camera.position +  Vector3{0, 0, -1};
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };

    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    ///////////
    
    Texture2D theoTexture = LoadTexture("assets/theo_1.png");
    bulletTexture = LoadTexture("assets/theo_1.png");
    blankTexture = LoadTexture("assets/blank.png");

    //////
    CreatePlayerEntity(&theoTexture);

    // Entity* blank = CreateEntity();
    // blank->flags = (Render | Collision);
    // blank->texture = &blankTexture;
    // blank->position.y = 2;
    // blank->scale = {1, 1, 1};
    // blank->collisionType = AABB;
    // blank->collisionSize = {1, 1};

    muiInit(&muCtx);

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
    muiProcessInput(&muCtx);
    mu_begin(&muCtx);

    if(showDebug == false) {
        for(int i = 0; i < MAX_ENTITY; i++) {

            if(entities[i].ControlFunction) {
                entities[i].ControlFunction(entities + i);
            }

            // EntityFlag f = entities[i].flags;

        }

        // collisions
        // @TODO: I'm not sure if it would be better to put it in main loop
        for(int ai = 0; ai < MAX_ENTITY; ai++) {
            Entity* a = entities + ai;
            if((a->flags & Collision) == 0) {
                continue;
            }

            for(int bi = ai + 1; bi < MAX_ENTITY; bi++) {
                Entity* b = entities + bi;
                if((b->flags & Collision) == 0) {
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
                    float rad = a->collisionSize.x + b->collisionSize;
                    collision = dist < rad * rad;
                }
                else if((a->collisionType == AABB   && b->collisionType == Circle) && 
                        (a->collisionType == Circle && b->collisionType == AABB)) {
                    Entity aabb   = a->collisionType == AABB ? a : b;
                    Entity circle = a->collisionType == Circle ? a : b;

                    float minX = aabb->position.x - aabb->collisionSize.x / 2;
                    float maxX = aabb->position.x + aabb->collisionSize.x / 2;
                    float maxY = aabb->position.y + aabb->collisionSize.y / 2;
                    float minY = aabb->position.y - aabb->collisionSize.y / 2;

                    float x = max(minX, min(circle->position.x, maxX));
                    float y = max(minY, min(circle->position.y, maxY));

                    float dist = (x - circle->position.x) * (x - circle->position.x) +
                                 (y - circle->position.y) * (y - circle->position.y);

                    collision = dis < circle->collisionSize.x * circle->collisionSize.x;
                } 

                if(collision) {
                    printf("Collision!!\n");
                }

            }
        }
    }

    if(controlCamera) {
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);
    }

    if(IsKeyPressed(KEY_Y)) {
        showDebug = !showDebug;
    }
    if(showDebug) {
        DebugWindow();
    }

    BeginDrawing();
    ClearBackground({219, 216, 225, 0});

    DrawFPS(0, 0);

    BeginMode3D(camera);
    {
        DrawGrid(15, 15);
        // DrawCube({0,0,0}, 1, 1, 1, RED);

        /// Render Entities
        for(int i = 0; i < MAX_ENTITY; i++) {
            Entity* e = entities + i;
            if(e->flags & Render) {
                assert(e->texture);

                DrawBillboard(camera, *e->texture, e->position, e->scale.x, WHITE);
            }
        }
    }
    EndMode3D();

    mu_end(&muCtx);
    muiRender(&muCtx);
    EndDrawing();
}
