#include "../../engine/sprites.h"
#include "../../media/fpg.h"

#include "raylib.h"

static Color raylibClearColor;
static RenderTexture2D target;
static const int gameScreenWidth = 320;
static const int gameScreenHeight = 240;

void setClearColor(uint8_t r, uint8_t g, uint8_t b) {
    raylibClearColor.r = r;
    raylibClearColor.g = g;
    raylibClearColor.b = b;
    raylibClearColor.a = 255;
}

void initDisplay() {
    const int windowWidth = 640;
    const int windowHeight = 480;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(windowWidth, windowHeight, "Game!");
    SetWindowMinSize(gameScreenWidth, gameScreenHeight);

    target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT); 

    SetTargetFPS(60);  

    setClearColor(0, 0, 0);

    BeginTextureMode(target);
    ClearBackground(raylibClearColor);
}

void HGL_init() {
    initDisplay();
    //initCD();
    //initMemory();
    //initFont();
    //initInput();

    initSprites();
    init_particles();
    
    init_fpgs();

}

void clear(struct m_image *dest);

void HGL_frame() {
    EndTextureMode();

    float scale = MIN((float)GetScreenWidth()/gameScreenWidth, (float)GetScreenHeight()/gameScreenHeight);
    float gameScreenWidthTimesScale = (float)gameScreenWidth*scale;
    float gameScreenHeightTimesScale = (float)gameScreenHeight*scale;
    
    BeginDrawing();
        DrawTexturePro(target.texture, (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
                        (Rectangle){ (GetScreenWidth() - gameScreenWidthTimesScale) * 0.5f, (GetScreenHeight() - gameScreenHeightTimesScale) * 0.5f,
                        gameScreenWidthTimesScale, gameScreenHeightTimesScale }, (Vector2){ 0, 0 }, 0.0f, WHITE);
    EndDrawing();

    BeginTextureMode(target);
    ClearBackground(raylibClearColor);
}

static Color fadeBlackColor = (Color) { 0, 0, 0, 255 };
static Color fadeWhiteColor = (Color) { 255, 255, 255, 255 };

void fadeToBlack(uint8_t fade) {
    fadeBlackColor.a = fade;
    DrawRectangle(0, 0, 320, 240, fadeBlackColor );
}

void fadeToWhite(uint8_t fade) {
    fadeWhiteColor.a = fade;
    DrawRectangle(0, 0, 320, 240, fadeWhiteColor );
}
