
#if WEB_BUILD
#include <emscripten/emscripten.h>
#endif

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

#include "common.cpp"

void UpdateDrawFrame();

Camera camera;
mu_Context muCtx;

int main()
{
    InitWindow(1700, 900, "Template");

    // Setup camera
    camera.position = Vector3{ 2.0f, 9.0f, 6.0f };
    camera.target = camera.position + Vector3{0, 0, -1};
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };

    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

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

    BeginDrawing();
    ClearBackground({219, 216, 225, 0});

    // DrawFPS(0, 0);

    style_window(&muCtx);

    BeginMode3D(camera);
    DrawGrid(15, 15);
    EndMode3D();

    mu_end(&muCtx);
    muiRender(&muCtx);
    EndDrawing();
}
