
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

#include "config.h"

#include "common.cpp"
#include "entity.cpp"

void UpdateDrawFrame();

Camera camera;
mu_Context muCtx;

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

    //////
    CreatePlayerEntity(&theoTexture);

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

    for(int i = 0; i < MAX_ENTITY; i++) {

        if(entities[i].ControlFunction) {
            entities[i].ControlFunction(entities + i);
        }

        EntityFlag f = entities[i].flags;

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
