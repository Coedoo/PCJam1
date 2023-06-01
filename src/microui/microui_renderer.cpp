#define _CRT_SECURE_NO_WARNINGS
#include "microui.h"

#define FONT_SPACING 2
#define SCROLL_SPEED 20

Font defaultFont; 
Texture2D iconsTexture;

#define RUNES_BUFFER_SIZE 32
int runesBuffer[RUNES_BUFFER_SIZE];

// Generated with rGuiIcons
// Icons data is defined by bit array (every bit represents one pixel)
// Those arrays are stored as unsigned int data arrays, so every array
// element defines 32 pixels (bits) of information
#define ICON_SIZE 16
#define ICON_COUNT 6
#define ELEMS_PER_ICON 8
u32 ICONS[] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,      // NONE
    0x00000000, 0x10080000, 0x04200810, 0x01800240, 0x02400180, 0x08100420, 0x00001008, 0x00000000,      // CLOSE
    0x00000000, 0x00000000, 0x10000000, 0x04000800, 0x01040200, 0x00500088, 0x00000020, 0x00000000,      // CHECK
    0x00000000, 0x00400000, 0x01c000c0, 0x07c003c0, 0x07c00fc0, 0x01c003c0, 0x004000c0, 0x00000000,      // COLLAPSED
    0x00000000, 0x00000000, 0x00000000, 0x0ff81ffc, 0x03e007f0, 0x008001c0, 0x00000000, 0x00000000,      // EXPANDED
    0x60000000, 0x78007000, 0x7e007c00, 0x7f807f00, 0x7fe07fc0, 0x7ff87ff0, 0x7ffe7ffc, 0x00007ffe,      // RESIZE
};


int muiGetTextWidth(mu_Font muFont, const char* text, int len){
    Font* font = (Font*) muFont;

    // for(int i = 0; i < text.len; i++) {
    //     r = text[i];

    //     index = GetGlyphIndex(font, r);

    //     if font.chars[index].advanceX == 0 {
    //         width += (i32)(font.recs[index].width) + FONT_SPACING;
    //     }
    //     else {
    //         width += font.chars[index].advanceX + FONT_SPACING;
    //     }
    // }

    return MeasureText(text, font->baseSize);
}

i32 muiGetTextHeight(mu_Font font) {
    Font* f = (Font*) font;
    return f->baseSize;
}

void muiInit(mu_Context* ret) {
    mu_init(ret);

    ret->text_width = muiGetTextWidth;
    ret->text_height = muiGetTextHeight;

    defaultFont = GetFontDefault();
    ret->style->font = (mu_Font) &defaultFont;

    // Create icons texture
    u32 texWidth = ICON_SIZE * ICON_COUNT;
    u32 texHeight = ICON_SIZE;

    u32 texData[ICON_SIZE * ICON_COUNT * ICON_SIZE];

    for (int iconIdx = 0; iconIdx <ICON_COUNT; iconIdx++) {
        for (int elemIdx  = 0; elemIdx <ELEMS_PER_ICON; elemIdx++) {
            u32 idx = iconIdx * ELEMS_PER_ICON + elemIdx;
            u32 v = ICONS[idx];

            u32 lower  =  v & 0x0000FFFF;
            u32 higher = (v & 0xFFFF0000) >> 16;

            for(int i = 0; i < 16; i++) {
                u32 color = (lower & (1 << (u32)i)) != 0 ? 0xFFFFFFFF : 0x00000000;

                u32 texX = iconIdx * ICON_SIZE + i;
                u32 texY = elemIdx * 2;

                texData[texY * texWidth + texX] = color;
            }

            for(int i = 0; i < 16; i++) {
                u32 color = (higher & (1 << (u32)i)) != 0 ? 0xFFFFFFFF : 0x00000000;

                u32 texX = iconIdx * ICON_SIZE + i;
                u32 texY = elemIdx * 2 + 1;

                texData[texY * texWidth + texX] = color;
            }
        }
    }

    u32 texId = rlLoadTexture(&texData, texWidth, texHeight, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);

    iconsTexture.id = texId;
    iconsTexture.width = texWidth;
    iconsTexture.height = texHeight;
    iconsTexture.mipmaps = 1;
    iconsTexture.format = RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
}

void muiProcessInput(mu_Context* ctx) {
    // mouse
    int posX = GetMouseX();
    int posY = GetMouseY();
    mu_input_mousemove(ctx, posX, posY);

    int mouseScroll = GetMouseWheelMove();
    mu_input_scroll(ctx, 0, i32(-SCROLL_SPEED * mouseScroll));

    // keys
    if      (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))   mu_input_mousedown(ctx, posX, posY, MU_MOUSE_LEFT);
    else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))  mu_input_mouseup(ctx, posX, posY, MU_MOUSE_LEFT);

    if      (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))   mu_input_mousedown(ctx, posX, posY, MU_MOUSE_RIGHT);
    else if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))  mu_input_mouseup(ctx, posX, posY, MU_MOUSE_RIGHT);

    if      (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE))   mu_input_mousedown(ctx, posX, posY, MU_MOUSE_RIGHT);
    else if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE))  mu_input_mouseup(ctx, posX, posY, MU_MOUSE_RIGHT);

    if      (IsKeyPressed(KEY_LEFT_SHIFT))   mu_input_keydown(ctx, MU_KEY_SHIFT);
    else if (IsKeyReleased(KEY_LEFT_SHIFT))  mu_input_keydown(ctx, MU_KEY_SHIFT);

    if      (IsKeyPressed(KEY_LEFT_CONTROL))   mu_input_keydown(ctx, MU_KEY_CTRL);
    else if (IsKeyReleased(KEY_LEFT_CONTROL))  mu_input_keydown(ctx, MU_KEY_CTRL);

    if      (IsKeyPressed(KEY_LEFT_ALT))   mu_input_keydown(ctx, MU_KEY_ALT);
    else if (IsKeyReleased(KEY_LEFT_ALT))  mu_input_keydown(ctx, MU_KEY_ALT);

    if      (IsKeyPressed(KEY_BACKSPACE))   mu_input_keydown(ctx, MU_KEY_BACKSPACE);
    else if (IsKeyReleased(KEY_BACKSPACE))  mu_input_keydown(ctx, MU_KEY_BACKSPACE);

    if      (IsKeyPressed(KEY_ENTER))   mu_input_keydown(ctx, MU_KEY_RETURN);
    else if (IsKeyReleased(KEY_ENTER))  mu_input_keydown(ctx, MU_KEY_RETURN);

    // text input
    // clear(&runesBuffer)

    // r := GetCharPressed()
    // for r != 0 {
    //     append(&runesBuffer, r)
    //     r = GetCharPressed()
    // }

    // str := utf8.runes_to_string(runesBuffer[:], context.temp_allocator)
    // mu_input_text(ctx, str)
}

Color ToColor(mu_Color c) {
    return {c.r, c.g, c.b, c.a};
}

void muiRender(mu_Context* muCtx) {
    rlEnableScissorTest();

    int winHeight = GetScreenHeight();

    mu_Command* cmd = NULL;
    while(mu_next_command(muCtx, &cmd)) {
        switch(cmd->type) {
            case MU_COMMAND_RECT: {
                mu_Rect rect = cmd->rect.rect;
                Rectangle r = {(float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h};
                DrawRectangleRec(r, ToColor(cmd->rect.color));
            }
            break;

            case MU_COMMAND_TEXT: {
                Font* f = (Font*) cmd->text.font;
                DrawTextEx(*f, cmd->text.str, Vector2{(float)cmd->text.pos.x, (float)cmd->text.pos.y}, f->baseSize, FONT_SPACING, ToColor(cmd->text.color));
            }
            break;

            case MU_COMMAND_ICON: {
                int x = (float)(cmd->icon.rect.x) + ((float)(cmd->icon.rect.w) - ICON_SIZE) / 2;
                int y = (float)(cmd->icon.rect.y) + ((float)(cmd->icon.rect.h) - ICON_SIZE) / 2;

                i32 index = cmd->icon.id;
                Rectangle src  = {float(ICON_SIZE * index), 0, ICON_SIZE, ICON_SIZE };
                Rectangle dest = {x, y, ICON_SIZE, ICON_SIZE};

                DrawTexturePro(iconsTexture, src, dest, Vector2{0, 0}, 0, ToColor(cmd->icon.color));
            }
            break;

            case MU_COMMAND_CLIP: {
                rlDrawRenderBatchActive();

                mu_Rect rect = cmd->clip.rect;
                rlScissor(rect.x, winHeight - (rect.y + rect.h), rect.w, rect.h);
            }
            break;

            case MU_COMMAND_JUMP: break;// Ignored;
        }
    }

    rlDisableScissorTest();
}

static int uint8_slider(mu_Context *ctx, unsigned char *value, int low, int high) {
  static float tmp;
  mu_push_id(ctx, &value, sizeof(value));
  tmp = *value;
  int res = mu_slider_ex(ctx, &tmp, low, high, 0, "%.0f", MU_OPT_ALIGNCENTER);
  *value = tmp;
  mu_pop_id(ctx);
  return res;
}

static void style_window(mu_Context *ctx) {
  static struct { const char *label; int idx; } colors[] = {
    { "text:",         MU_COLOR_TEXT        },
    { "border:",       MU_COLOR_BORDER      },
    { "windowbg:",     MU_COLOR_WINDOWBG    },
    { "titlebg:",      MU_COLOR_TITLEBG     },
    { "titletext:",    MU_COLOR_TITLETEXT   },
    { "panelbg:",      MU_COLOR_PANELBG     },
    { "button:",       MU_COLOR_BUTTON      },
    { "buttonhover:",  MU_COLOR_BUTTONHOVER },
    { "buttonfocus:",  MU_COLOR_BUTTONFOCUS },
    { "base:",         MU_COLOR_BASE        },
    { "basehover:",    MU_COLOR_BASEHOVER   },
    { "basefocus:",    MU_COLOR_BASEFOCUS   },
    { "scrollbase:",   MU_COLOR_SCROLLBASE  },
    { "scrollthumb:",  MU_COLOR_SCROLLTHUMB },
    { NULL }
  };

  if (mu_begin_window(ctx, "Style Editor", mu_rect(350, 250, 300, 240))) {
    int sw = mu_get_current_container(ctx)->body.w * 0.14;
    int rows[] = { 80, sw, sw, sw, sw, -1 };
    mu_layout_row(ctx, 6, rows, 0);
    for (int i = 0; colors[i].label; i++) {
      mu_label(ctx, colors[i].label);
      uint8_slider(ctx, &ctx->style->colors[i].r, 0, 255);
      uint8_slider(ctx, &ctx->style->colors[i].g, 0, 255);
      uint8_slider(ctx, &ctx->style->colors[i].b, 0, 255);
      uint8_slider(ctx, &ctx->style->colors[i].a, 0, 255);
      mu_draw_rect(ctx, mu_layout_next(ctx), ctx->style->colors[i]);
    }
    mu_end_window(ctx);
  }
}
