#include <stdio.h>
#include <stdlib.h>
#include "sdl3_backend.h"
#include "../../engine/sprites.h" // For Tsprite
#include "../../input/input.h" // For PADSTATE
#include "../../media/fpg.h" // For fpg array

#define STB_IMAGE_IMPLEMENTATION
#include "../../external/stb_image.h"

#include <SDL3/SDL.h>

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static const int gameScreenWidth = 320;
static const int gameScreenHeight = 240;
static const int windowWidth = 640;
static const int windowHeight = 480;

static const double targetFPS = 60.0;
static const double targetFrameTime = 1.0 / 60.0;
static uint64_t lastFrameTime = 0;

static uint16_t button_state = 0;
static PADSTATE pad_state = {0};

void HGL_init() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return;
    }

    //SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d12");  
    //SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d11");  
    //SDL_SetHint(SDL_HINT_RENDER_DRIVER, "vulkan");  
    //SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");  
    if (!SDL_CreateWindowAndRenderer("SDL3 Backend", windowWidth, windowHeight, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("SDL_CreateWindowAndRenderer failed: %s", SDL_GetError());
        return;
    }

    const char *name = SDL_GetRendererName(renderer);  
    printf("Using renderer: %s\n", name);  

    SDL_SetHint("SDL_RENDER_SCALE_QUALITY", "linear");
    SDL_SetRenderLogicalPresentation(renderer, gameScreenWidth, gameScreenHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    initInput();
    initSprites();
    init_particles();
    init_fpgs();
    
    lastFrameTime = SDL_GetTicks();
}

double HGL_getTime() {
    return SDL_GetTicks() / 1000.0;
}

void setMainLoopCallback(void (*mainLoop)()) {
    while (1) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                goto quit;
            }
            // Handle input events here if needed, or in HGL_frame
        }
        
        mainLoop();
    }
    
quit:
    SDL_Quit();
}

static uint8_t clearR = 0, clearG = 0, clearB = 0;

void HGL_frame() {
    SDL_RenderPresent(renderer);
    
    /*uint64_t currentTime = SDL_GetTicks();
    double frameTime = (currentTime - lastFrameTime) / 1000.0;
    
    if (frameTime < targetFrameTime) {
        SDL_Delay((uint32_t)((targetFrameTime - frameTime) * 1000.0));
        lastFrameTime = SDL_GetTicks();
    } else {
        lastFrameTime = currentTime;
    }*/
    
    SDL_SetRenderDrawColor(renderer, clearR, clearG, clearB, 255);
    SDL_RenderClear(renderer);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            exit(0);
        }
        
        if (event.type == SDL_EVENT_KEY_DOWN) {
            if (event.key.key == SDLK_RETURN && (event.key.mod & SDL_KMOD_ALT)) {
                SDL_WindowFlags flags = SDL_GetWindowFlags(window);
                if (flags & SDL_WINDOW_FULLSCREEN) {
                    SDL_SetWindowFullscreen(window, 0);
                } else {
                    SDL_SetWindowFullscreen(window, 1);
                }
            }
        }
        if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
            int down = (event.type == SDL_EVENT_KEY_DOWN);
            switch (event.key.key) {
                case SDLK_X: if(down) button_state |= PAD_CROSS; else button_state &= ~PAD_CROSS; break;
                case SDLK_Z: if(down) button_state |= PAD_SQUARE; else button_state &= ~PAD_SQUARE; break;
                case SDLK_RETURN: if(down) button_state |= PAD_START; else button_state &= ~PAD_START; break;
                case SDLK_UP: if(down) button_state |= PAD_UP; else button_state &= ~PAD_UP; break;
                case SDLK_DOWN: if(down) button_state |= PAD_DOWN; else button_state &= ~PAD_DOWN; break;
                case SDLK_LEFT: if(down) button_state |= PAD_LEFT; else button_state &= ~PAD_LEFT; break;
                case SDLK_RIGHT: if(down) button_state |= PAD_RIGHT; else button_state &= ~PAD_RIGHT; break;
            }
        }
    }
}

void setClearColor(uint8_t r, uint8_t g, uint8_t b) {
    clearR = r; clearG = g; clearB = b;
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
}

void fadeToBlack(uint8_t fade) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, fade);
    SDL_FRect rect = {0, 0, (float)gameScreenWidth, (float)gameScreenHeight};
    SDL_RenderFillRect(renderer, &rect);
}

void fadeToWhite(uint8_t fade) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, fade);
    SDL_FRect rect = {0, 0, (float)gameScreenWidth, (float)gameScreenHeight};
    SDL_RenderFillRect(renderer, &rect);
}

void GetSpriteFromDisc(char *filename, SPRITE *sprite) {
    int width, height, channels;
    char fullFilename[256];
    snprintf(fullFilename, sizeof(fullFilename), "%s.png", filename);
    
    unsigned char *data = stbi_load(fullFilename, &width, &height, &channels, 4);
    if (!data) {
        printf("Failed to load image: %s\n", fullFilename);
        return;
    }

    SDL_Surface* surface = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA32, data, width * 4);
    if (surface) {
        sprite->texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!sprite->texture) {
            printf("Failed to create texture from surface for %s: %s\n", fullFilename, SDL_GetError());
        } else {
            SDL_SetTextureBlendMode(sprite->texture, SDL_BLENDMODE_BLEND);
            SDL_SetTextureScaleMode(sprite->texture, SDL_SCALEMODE_NEAREST);
        }
        SDL_DestroySurface(surface);
    } else {
        printf("Failed to create surface for %s: %s\n", fullFilename, SDL_GetError());
    }
    stbi_image_free(data);

    sprite->u = 0;
    sprite->v = 0;
    sprite->w = width;
    sprite->h = height;
    sprite->tex_w = width;
    sprite->tex_h = height;
}

void GetSpriteFromMemory(uint8_t *data, SPRITE *sprite) {
    // ...
}

void CopySprite(SPRITE *original, SPRITE *sprite) {
    *sprite = *original;
}

void SetSpriteDimensions(SPRITE *sprite, float u, float v, float w, float h) {
    sprite->u = u;
    sprite->v = v;
    sprite->w = w;
    sprite->h = h;
}

void initInput() {
}

PADSTATE* getPadState(int player) {
    pad_state.btn = button_state;
    return &pad_state;
}

uint16_t getButtons(int player) {
    return (player == 0) ? button_state : 0;
}

void draw_sprite(Tsprite *spr) {
    if (spr == NULL || spr->graph <= 0 || spr->id < 0) return;
    SPRITE* sprite = fpg[spr->file]->map[spr->graph]->image;
    if (sprite == NULL || sprite->texture == NULL) return;

    float x = spr->x;
    float y = spr->y;
    float w = sprite->w;
    float h = sprite->h;
    
    SDL_FRect srcRect;
    srcRect.x = (float)sprite->u;
    srcRect.y = (float)sprite->v;
    srcRect.w = (float)sprite->w;
    srcRect.h = (float)sprite->h;

    SDL_FlipMode flip = SDL_FLIP_NONE;
    if (spr->flags & 1) flip |= SDL_FLIP_HORIZONTAL;
    if (spr->flags & 2) flip |= SDL_FLIP_VERTICAL;

    float scale = spr->size_x / 4096.0f;
    w *= scale;
    h *= scale;
    double angle = (spr->angle * 360.0f) / 4096.0f;
    
    SDL_FRect dstRect;
    dstRect.x = x - w/2;
    dstRect.y = y - h/2;
    dstRect.w = w;
    dstRect.h = h;
    
    SDL_FPoint center;
    center.x = w/2;
    center.y = h/2;

    SDL_RenderTextureRotated(renderer, sprite->texture, &srcRect, &dstRect, angle, &center, flip);    
}

static inline void draw_sprite_fast_generic(Tsprite *spr) {
    if (spr == NULL || spr->graph <= 0 || spr->id < 0) return;
    SPRITE* sprite = fpg[spr->file]->map[spr->graph]->image;
    if (sprite == NULL || sprite->texture == NULL) return;

    float x = spr->x;
    float y = spr->y;
    float w = sprite->w;
    float h = sprite->h;
    
    SDL_FRect srcRect;
    srcRect.x = (float)sprite->u;
    srcRect.y = (float)sprite->v;
    srcRect.w = (float)sprite->w;
    srcRect.h = (float)sprite->h;

    SDL_FRect dstRect;
    dstRect.x = x;
    dstRect.y = y;
    dstRect.w = w;
    dstRect.h = h;
    
    SDL_RenderTexture(renderer, sprite->texture, &srcRect, &dstRect);
}

void draw_sprite_fast(Tsprite *spr) { draw_sprite_fast_generic(spr); }
void draw_tile16_fast(Tsprite *spr) { draw_sprite_fast_generic(spr); }
void draw_tile8_fast(Tsprite *spr) { draw_sprite_fast_generic(spr); }


