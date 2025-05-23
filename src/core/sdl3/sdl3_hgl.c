#include <SDL3/SDL.h>
#include "../../engine/sprites.h"
#include "../../media/fpg.h"
// It's good practice to include a header for the current file, if one exists or is planned
// #include "sdl3_hgl.h" 

// Static variables for screen dimensions and clear color
static int gameScreenWidth = 320;
static int gameScreenHeight = 240;
static SDL_Color clearColor = {0, 0, 0, 255}; // Default to black

// SDL specific variables
SDL_Window *window = NULL;     // Made non-static for potential external access if ever needed (though not typical)
SDL_Renderer *renderer = NULL; // Made non-static for access from sdl3_hgl_file.c and sdl3_sprites.c

void setClearColor(uint8_t r, uint8_t g, uint8_t b) {
    clearColor.r = r;
    clearColor.g = g;
    clearColor.b = b;
    clearColor.a = 255; // Alpha is always opaque for clear color
}

// startFrame is now declared in hgl.h, so it's part of the public API for this backend
void startFrame() {
    if (!renderer) {
        SDL_Log("startFrame: Renderer is NULL!");
        return;
    }

    // Handle window resizing - get current window size
    int currentWidth, currentHeight;
    SDL_GetWindowSize(window, &currentWidth, &currentHeight);

    // Update viewport to match window size - this makes the content scale with the window
    SDL_Rect viewport;
    viewport.x = 0;
    viewport.y = 0;
    viewport.w = currentWidth;
    viewport.h = currentHeight;
    SDL_RenderSetViewport(renderer, &viewport);

    // Set logical size for rendering, this will scale the 320x240 content
    // to the window size, maintaining aspect ratio.
    // SDL_RenderSetLogicalSize(renderer, gameScreenWidth, gameScreenHeight);
    // For more direct control as per original, we might use SDL_RenderSetScale
    // and manage aspect ratio manually if needed.
    // Let's assume for now direct scaling without letterboxing/pillarboxing
    // SDL_RenderSetScale(renderer, (float)currentWidth / gameScreenWidth, (float)currentHeight / gameScreenHeight);


    // Clear the screen
    SDL_SetRenderDrawColor(renderer, clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    SDL_RenderClear(renderer);

    // Prepare for rendering (not strictly necessary in SDL3 like it was in some older APIs)
}

// initDisplay is an internal helper, remains static
static int initDisplay() {
    // Initialize SDL_VIDEO if not already done by a general SDL_Init() elsewhere
    // SDL_Init(SDL_INIT_VIDEO) is fine if this is the first SDL call.
    // If SDL_Init() was called globally, SDL_InitSubSystem(SDL_INIT_VIDEO) is also an option.
    if (!SDL_WasInit(SDL_INIT_VIDEO)) {
        if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
            SDL_Log("SDL Video subsystem could not initialize! SDL_Error: %s\n", SDL_GetError());
            return -1;
        }
    }
    
    // Also ensure Events subsystem is up for window events, quit, etc.
    if (!SDL_WasInit(SDL_INIT_EVENTS)) {
        if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0) {
            SDL_Log("SDL Events subsystem could not initialize! SDL_Error: %s\n", SDL_GetError());
            // Continue if video succeeded, but log warning. Or return -1.
            // return -1; 
        }
    }


    window = SDL_CreateWindow("HGL Game",
                              gameScreenWidth, gameScreenHeight,
                              SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        // SDL_Quit(); // Don't call SDL_Quit() here, let caller handle full shutdown
        return -1;
    }

    renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        SDL_Log("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        window = NULL;
        // SDL_Quit(); // Let caller handle full shutdown
        return -1;
    }
    return 0;
}

// HGL_init is declared in hgl.h
int HGL_init() {
    // Overall SDL initialization (if not done elsewhere, e.g. for audio)
    // If other parts of the engine call SDL_Init with other flags, this might be redundant
    // or could be SDL_Init(0) to just ensure it's up.
    // For now, assume HGL_init is responsible for core video and events.
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD) < 0) { // Added GAMEPAD here
         SDL_Log("SDL_Init failed: %s", SDL_GetError());
         return -1; // Crucial: return failure
    }


    if (initDisplay() != 0) {
        SDL_Log("initDisplay failed during HGL_init.");
        SDL_Quit(); // Clean up SDL if display init failed
        return -1;
    }

    // Initialize other engine components that depend on SDL
    // initSprites() is declared in sprites.h. It might need access to the renderer.
    // If initSprites (and others like init_fpgs) are generic, they should be called
    // by the game's own initialization logic (Game_init).
    // If they are backend-specific, they could be here.
    // The prompt for sdl3_hgl.c placed initSprites() here.
    initSprites(); 
    // init_particles(); // Placeholder
    // init_fpgs();    // Placeholder, fpg.c has its own init_fpgs

    // Call startFrame once to clear the screen initially
    startFrame();

    return 0; // Success
}

// HGL_frame is declared in hgl.h
void HGL_frame() {
    if (renderer) {
        SDL_RenderPresent(renderer);
    } else {
        SDL_Log("HGL_frame: Renderer is NULL!");
    }
    // startFrame() is now called explicitly in the main loop *before* drawing.
    // This function is now only for presenting the drawn frame.
}

// HGL_quit is declared in hgl.h
void HGL_quit() {
    SDL_Log("HGL_quit called.");
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    SDL_Quit(); // This shuts down all SDL subsystems
}


void fadeToBlack(uint8_t fade) {
    if (!renderer) return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, fade);
    
    int w, h;
    SDL_GetRenderOutputSize(renderer, &w, &h); // Get the current render output size
    SDL_Rect screenRect = {0, 0, w, h};
    SDL_RenderFillRect(renderer, &screenRect);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE); // Reset blend mode
}

void fadeToWhite(uint8_t fade) {
    if (!renderer) return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, fade);

    int w, h;
    SDL_GetRenderOutputSize(renderer, &w, &h); // Get the current render output size
    SDL_Rect screenRect = {0, 0, w, h};
    SDL_RenderFillRect(renderer, &screenRect);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE); // Reset blend mode
}

// Optional: A function to clean up SDL resources
void HGL_quit() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    SDL_Quit();
}
