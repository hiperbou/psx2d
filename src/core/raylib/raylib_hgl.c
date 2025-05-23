#include "../../engine/sprites.h"
#include "../../media/fpg.h"
#include "../hgl.h" // Added for HGL function prototypes

#include <raylib.h>
#include "rlgl.h"

static Color raylibClearColor;

static const int gameScreenWidth = 320;
static const int gameScreenHeight = 240;

void setClearColor(uint8_t r, uint8_t g, uint8_t b) {
    raylibClearColor.r = r;
    raylibClearColor.g = g;
    raylibClearColor.b = b;
    raylibClearColor.a = 255;
}

void HGL_start_frame() { // Renamed and no longer static
    //int screen_width = (GetScreenWidth() / gameScreenWidth) * gameScreenWidth;  
    //int screen_height = (GetScreenHeight() / gameScreenHeight) * gameScreenHeight; 
    int screen_width = GetScreenWidth();  
    int screen_height = GetScreenHeight(); 

    int virtual_width = gameScreenWidth;
    int virtual_height = gameScreenHeight;
      
    float targetAspectRatio = virtual_width/(float)virtual_height;
      
    int width = screen_width ;
    int height = (int)(width / targetAspectRatio + 0.5f);

    if (height > screen_height )
    {
        height = screen_height ;
        width = (int)(height * targetAspectRatio + 0.5f);
    }

    int vp_x = (screen_width  * 0.5) - (width * 0.5);
    int vp_y = (screen_height * 0.5) - (height * 0.5);

    float scale_x = screen_width / (float)virtual_width;
    float scale_y = screen_height / (float)virtual_height;
      
    rlViewport(vp_x,vp_y,width,height);

    rlMatrixMode(RL_PROJECTION);
    rlLoadIdentity();

    rlOrtho(0, screen_width, screen_height, 0, 0.0f, 1.0f);

    rlMatrixMode(RL_MODELVIEW);

    BeginDrawing();
        rlScalef(scale_x, scale_y, 1.0f);
        ClearBackground(raylibClearColor);
}


void initDisplay() {
    const int windowWidth = 640;
    const int windowHeight = 480;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(windowWidth, windowHeight, "Game!");
    SetWindowMinSize(gameScreenWidth, gameScreenHeight);

    SetTargetFPS(60);  

    setClearColor(0, 0, 0);
    // Removed the call to startFrame() from here
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

void HGL_frame() {
    EndDrawing();
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
