#include <SDL3/SDL.h>
#include "../../engine/sprites.h" // Assuming this will be adapted for SDL_Texture if it holds image data
#include "../../media/fpg.h"     // Assuming this will be adapted for SDL_Texture if it holds image data

// SDL specific globals
static SDL_Window* sdlWindow = NULL;
static SDL_Renderer* sdlRenderer = NULL;
static SDL_Color sdlClearColor = {0, 0, 0, 255};

static const int gameScreenWidth = 320;
static const int gameScreenHeight = 240;

void setClearColor(uint8_t r, uint8_t g, uint8_t b) {
    sdlClearColor.r = r;
    sdlClearColor.g = g;
    sdlClearColor.b = b;
    sdlClearColor.a = 255;
}

static void startFrame() {
    // Get current window size
    int windowWidth, windowHeight;
    SDL_GetWindowSize(sdlWindow, &windowWidth, &windowHeight);

    // Calculate aspect ratio for scaling
    float targetAspectRatio = (float)gameScreenWidth / gameScreenHeight;
    
    int finalWidth = windowWidth;
    int finalHeight = (int)((float)finalWidth / targetAspectRatio + 0.5f);

    if (finalHeight > windowHeight) {
        finalHeight = windowHeight;
        finalWidth = (int)((float)finalHeight * targetAspectRatio + 0.5f);
    }

    // Calculate viewport position (centered)
    int vp_x = (windowWidth / 2) - (finalWidth / 2);
    int vp_y = (windowHeight / 2) - (finalHeight / 2);

    SDL_Rect viewport = { vp_x, vp_y, finalWidth, finalHeight };
    SDL_RenderSetViewport(sdlRenderer, &viewport);
    
    // Set logical size for automatic scaling of rendering operations
    // This simplifies scaling compared to manual matrix operations in rlgl
    SDL_RenderSetLogicalSize(sdlRenderer, gameScreenWidth, gameScreenHeight);

    SDL_SetRenderDrawColor(sdlRenderer, sdlClearColor.r, sdlClearColor.g, sdlClearColor.b, sdlClearColor.a);
    SDL_RenderClear(sdlRenderer);
}

void initDisplay() {
    const int windowWidth = 640;
    const int windowHeight = 480;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    sdlWindow = SDL_CreateWindow("Game!", windowWidth, windowHeight, SDL_WINDOW_RESIZABLE);
    if (sdlWindow == NULL) {
        SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    SDL_SetWindowMinimumSize(sdlWindow, gameScreenWidth, gameScreenHeight);

    // Create renderer
    // SDL_RENDERER_ACCELERATED for hardware acceleration
    // SDL_RENDERER_PRESENTVSYNC for vsync
    sdlRenderer = SDL_CreateRenderer(sdlWindow, NULL, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (sdlRenderer == NULL) {
        SDL_Log("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(sdlWindow);
        SDL_Quit();
        return;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear"); // Make scaled rendering look smoother.

    setClearColor(0, 0, 0); // Default clear color
    
    // Initial frame setup
    // In SDL, the "drawing" happens between RenderClear and RenderPresent.
    // startFrame() clears the screen and sets up viewport/scaling.
    // No equivalent to Raylib's BeginDrawing()/EndDrawing() in the same way for global state.
    // The HGL_frame will handle RenderPresent.
}

void HGL_init() {
    initDisplay();
    //initCD(); // Assuming these are not graphics related for now
    //initMemory();
    //initFont();
    //initInput(); // This will need an SDL equivalent later

    initSprites(); // This function will need to be SDL-aware
    init_particles(); // This function might need SDL_Renderer
    
    init_fpgs(); // This function will need to be SDL-aware
}

void HGL_frame() {
    // In SDL, the typical loop is:
    // 1. Handle events
    // 2. Update game state
    // 3. Render
    //    a. Clear screen (done in startFrame or at the beginning of render block)
    //    b. Draw objects
    //    c. Present renderer
    
    // startFrame() should be called to set up viewport and clear for the new frame.
    startFrame(); 
    
    // ... game drawing calls would happen here ...

    SDL_RenderPresent(sdlRenderer); // Update screen with rendering operations
}

static SDL_Color fadeColor = {0, 0, 0, 255};

void fadeToBlack(uint8_t fade) {
    fadeColor.r = 0;
    fadeColor.g = 0;
    fadeColor.b = 0;
    fadeColor.a = fade;
    SDL_SetRenderDrawColor(sdlRenderer, fadeColor.r, fadeColor.g, fadeColor.b, fadeColor.a);
    SDL_Rect fullScreenRect = {0, 0, gameScreenWidth, gameScreenHeight};
    SDL_RenderFillRect(sdlRenderer, &fullScreenRect);
}

void fadeToWhite(uint8_t fade) {
    fadeColor.r = 255;
    fadeColor.g = 255;
    fadeColor.b = 255;
    fadeColor.a = fade;
    SDL_SetRenderDrawColor(sdlRenderer, fadeColor.r, fadeColor.g, fadeColor.b, fadeColor.a);
    SDL_Rect fullScreenRect = {0, 0, gameScreenWidth, gameScreenHeight};
    SDL_RenderFillRect(sdlRenderer, &fullScreenRect);
}

// Need to add cleanup function for SDL
void HGL_close() {
    // Destroy FPGs, sprites, particles if they have SDL resources
    // close_fpgs(); // Example
    // close_sprites(); // Example
    // close_particles(); // Example

    if (sdlRenderer) {
        SDL_DestroyRenderer(sdlRenderer);
        sdlRenderer = NULL;
    }
    if (sdlWindow) {
        SDL_DestroyWindow(sdlWindow);
        sdlWindow = NULL;
    }
    SDL_Quit();
}

// Accessor for the renderer, might be needed by other modules (e.g., sprite rendering)
SDL_Renderer* HGL_GetRenderer() {
    return sdlRenderer;
}
```
