#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "opengl_backend.h"
#include "../../engine/sprites.h" // For Tsprite
#include "../../input/input.h" // For PADSTATE
#include "../../media/fpg.h" // For fpg array
#include <GLFW/glfw3.h>

#ifdef _WIN32
    #include <windows.h>  // For Sleep()
#else
    #include <time.h>     // For nanosleep()
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "../../external/stb_image.h"

static GLFWwindow* window = NULL;

static const int gameScreenWidth = 320;
static const int gameScreenHeight = 240;
static const int windowWidth = 640;  // 2x scale
static const int windowHeight = 480; // 2x scale

static const double targetFPS = 60.0;
static const double targetFrameTime = 1.0 / 60.0;  // ~16.67ms per frame
static double lastFrameTime = 0.0;

static int vpX = 0, vpY = 0, vpW = 0, vpH = 0;
static uint8_t clearR = 0, clearG = 0, clearB = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    float targetAspect = (float)gameScreenWidth / (float)gameScreenHeight;
    float windowAspect = (float)width / (float)height;
    
    if (windowAspect > targetAspect) {
        // Window is wider than game, letterbox horizontally
        vpH = height;
        vpW = (int)(height * targetAspect);
        vpX = (width - vpW) / 2;
        vpY = 0;
    } else {
        // Window is taller than game, letterbox vertically
        vpW = width;
        vpH = (int)(width / targetAspect);
        vpX = 0;
        vpY = (height - vpH) / 2;
    }
    
    glViewport(vpX, vpY, vpW, vpH);
}

void HGL_init() {
    if (!glfwInit()) {
        return;
    }

    window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL Backend", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    framebuffer_size_callback(window, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, gameScreenWidth, gameScreenHeight, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    initInput();

    initSprites();
    init_particles();
    
    init_fpgs();
    
    lastFrameTime = glfwGetTime();
    
    #ifdef _WIN32
    timeBeginPeriod(1);
    #endif
}

void setMainLoopCallback(void (*mainLoop)()) {
    while (!glfwWindowShouldClose(window)) {
        mainLoop();
    }
    
    #ifdef _WIN32
    timeEndPeriod(1);
    #endif
    
    glfwTerminate();
}

void HGL_frame() {
    glfwSwapBuffers(window);
    glfwPollEvents();
    
    double currentTime = glfwGetTime();
    double frameTime = currentTime - lastFrameTime;
    
    // Wait if frame finished too quickly
    if (frameTime < targetFrameTime) {
        double waitTime = targetFrameTime - frameTime;
        
        // Sleep for 95% of the wait time
        double sleepTime = waitTime * 0.95;
        
        #ifdef _WIN32
            if (sleepTime > 0.001) {
                Sleep((DWORD)(sleepTime * 1000.0));
            }
        #else
            if (sleepTime > 0.000001) {
                struct timespec req;
                req.tv_sec = (time_t)sleepTime;
                req.tv_nsec = (long)((sleepTime - req.tv_sec) * 1000000000L);
                nanosleep(&req, NULL);
            }
        #endif
        
        // Busy-wait for the remaining 5% for precise timing
        double targetTime = currentTime + waitTime;
        while (glfwGetTime() < targetTime) {
            // Tight loop for precise timing (only ~0.8ms)
        }
        
        lastFrameTime = glfwGetTime();
    } else {
        lastFrameTime = currentTime;
    }
    
    glDisable(GL_SCISSOR_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glEnable(GL_SCISSOR_TEST);
    glScissor(vpX, vpY, vpW, vpH);
    glClearColor(clearR / 255.0f, clearG / 255.0f, clearB / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}


void setClearColor(uint8_t r, uint8_t g, uint8_t b) {
    clearR = r;
    clearG = g;
    clearB = b;
}

void fadeToBlack(uint8_t fade) {
    glDisable(GL_TEXTURE_2D);
    glColor4f(0.0f, 0.0f, 0.0f, fade / 255.0f);
    glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(gameScreenWidth, 0);
        glVertex2f(gameScreenWidth, gameScreenHeight);
        glVertex2f(0, gameScreenHeight);
    glEnd();
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
}

void fadeToWhite(uint8_t fade) {
    glDisable(GL_TEXTURE_2D);
    glColor4f(1.0f, 1.0f, 1.0f, fade / 255.0f);
    glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(gameScreenWidth, 0);
        glVertex2f(gameScreenWidth, gameScreenHeight);
        glVertex2f(0, gameScreenHeight);
    glEnd();
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
}

void GetSpriteFromDisc(char *filename, SPRITE *sprite) {
    int width, height, channels;
    char fullFilename[256];
    
    snprintf(fullFilename, sizeof(fullFilename), "%s.png", filename);
    
    printf("Loading texture: %s\n", fullFilename);
    unsigned char *data = stbi_load(fullFilename, &width, &height, &channels, 4);

    if (data == NULL || sprite == NULL) {
        printf("ERROR: Failed to load texture %s: %s\n", fullFilename, stbi_failure_reason());
        if(data) stbi_image_free(data);
        return;
    }

    printf("Loaded texture %s: %dx%d, %d channels\n", fullFilename, width, height, channels);

    glGenTextures(1, &sprite->texture_id);
    glBindTexture(GL_TEXTURE_2D, sprite->texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    sprite->u = 0;
    sprite->v = 0;
    sprite->w = width;
    sprite->h = height;
    sprite->tex_w = width;
    sprite->tex_h = height;
    
    printf("Created texture ID: %u\n", sprite->texture_id);
}

void GetSpriteFromMemory(uint8_t *data, /*int len,*/ SPRITE *sprite) {
    /*int width, height, channels;
    unsigned char *img_data = stbi_load_from_memory(data, len, &width, &height, &channels, 4);

    if (img_data == NULL || sprite == NULL) {
        if(img_data) stbi_image_free(img_data);
        return;
    }

    glGenTextures(1, &sprite->texture_id);
    glBindTexture(GL_TEXTURE_2D, sprite->texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);

    stbi_image_free(img_data);

    sprite->u = 0;
    sprite->v = 0;
    sprite->w = width;
    sprite->h = height;*/
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

static uint16_t button_state = 0;
static PADSTATE pad_state = {0};

static int isFullscreen = 0;
static int savedX = 100;
static int savedY = 100;
static int savedW = 640;
static int savedH = 480;

void key_callback(GLFWwindow* win, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ENTER && (mods & GLFW_MOD_ALT)) {
            if (!isFullscreen) {
                // Save current window state
                glfwGetWindowPos(win, &savedX, &savedY);
                glfwGetWindowSize(win, &savedW, &savedH);
                
                // Switch to borderless fullscreen
                GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                
                glfwSetWindowAttrib(win, GLFW_DECORATED, GLFW_FALSE);
                glfwSetWindowAttrib(win, GLFW_FLOATING, GLFW_TRUE);
                glfwSetWindowPos(win, 0, 0);
                glfwSetWindowSize(win, mode->width+1, mode->height);
                isFullscreen = 1;
            } else {
                // Restore windowed mode
                glfwSetWindowAttrib(win, GLFW_DECORATED, GLFW_TRUE);
                glfwSetWindowAttrib(win, GLFW_FLOATING, GLFW_FALSE);
                glfwSetWindowSize(win, savedW, savedH);
                glfwSetWindowPos(win, savedX, savedY);
                isFullscreen = 0;
            }
        }
        if (key == GLFW_KEY_X) button_state |= PAD_CROSS;
        if (key == GLFW_KEY_Z) button_state |= PAD_SQUARE;
        if (key == GLFW_KEY_ENTER) button_state |= PAD_START;
        if (key == GLFW_KEY_UP) button_state |= PAD_UP;
        if (key == GLFW_KEY_DOWN) button_state |= PAD_DOWN;
        if (key == GLFW_KEY_LEFT) button_state |= PAD_LEFT;
        if (key == GLFW_KEY_RIGHT) button_state |= PAD_RIGHT;
    } else if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_X) button_state &= ~PAD_CROSS;
        if (key == GLFW_KEY_Z) button_state &= ~PAD_SQUARE;
        if (key == GLFW_KEY_ENTER) button_state &= ~PAD_START;
        if (key == GLFW_KEY_UP) button_state &= ~PAD_UP;
        if (key == GLFW_KEY_DOWN) button_state &= ~PAD_DOWN;
        if (key == GLFW_KEY_LEFT) button_state &= ~PAD_LEFT;
        if (key == GLFW_KEY_RIGHT) button_state &= ~PAD_RIGHT;
    }
}

void initInput() {
    glfwSetKeyCallback(window, key_callback);
}

PADSTATE* getPadState(int player) {
    return 0;//(PADSTATE*)padbuff[player];
}

uint16_t getButtons(int player) {
    if (player == 0) {
        return button_state;
    }
    return 0;
}

void draw_sprite(Tsprite *spr) {
    if (spr == NULL || spr->graph <= 0 || spr->id < 0) return;
    SPRITE* sprite = fpg[spr->file]->map[spr->graph]->image;
    if (sprite == NULL) return;

    int mirroredHorizontal = spr->flags & 1;
    int mirroredVertical = spr->flags & 2;

    float scale = spr->size_x / 4096.0f;
    float angle = (spr->angle * 360.0f) / 4096.0f;
    float w = sprite->w * scale;
    float h = sprite->h * scale;
    
    float u = (float)sprite->u / (float)sprite->tex_w;
    float v = (float)sprite->v / (float)sprite->tex_h;
    float u2 = (float)(sprite->u + sprite->w) / (float)sprite->tex_w;
    float v2 = (float)(sprite->v + sprite->h) / (float)sprite->tex_h;
    
    if (mirroredHorizontal) {
        float temp = u;
        u = u2;
        u2 = temp;
    }
    if (mirroredVertical) {
        float temp = v;
        v = v2;
        v2 = temp;
    }

    glPushMatrix();
    glTranslatef(spr->x, spr->y, 0);
    glRotatef(angle, 0, 0, 1);
    glTranslatef(-w/2, -h/2, 0);

    glBindTexture(GL_TEXTURE_2D, sprite->texture_id);
    glBegin(GL_QUADS);
        glTexCoord2f(u, v); glVertex2f(0, 0);
        glTexCoord2f(u2, v); glVertex2f(w, 0);
        glTexCoord2f(u2, v2); glVertex2f(w, h);
        glTexCoord2f(u, v2); glVertex2f(0, h);
    glEnd();
    glPopMatrix();
}

static inline void draw_sprite_fast_generic(Tsprite *spr) {
    if (spr == NULL || spr->graph <= 0 || spr->id < 0) return;
    SPRITE* sprite = fpg[spr->file]->map[spr->graph]->image;
    if (sprite == NULL) return;

    float w = sprite->w;
    float h = sprite->h;

    float u = (float)sprite->u / (float)sprite->tex_w;
    float v = (float)sprite->v / (float)sprite->tex_h;
    float u2 = (float)(sprite->u + sprite->w) / (float)sprite->tex_w;
    float v2 = (float)(sprite->v + sprite->h) / (float)sprite->tex_h;

    glBindTexture(GL_TEXTURE_2D, sprite->texture_id);
    glBegin(GL_QUADS);
        glTexCoord2f(u, v); glVertex2f(spr->x, spr->y);
        glTexCoord2f(u2, v); glVertex2f(spr->x + w, spr->y);
        glTexCoord2f(u2, v2); glVertex2f(spr->x + w, spr->y + h);
        glTexCoord2f(u, v2); glVertex2f(spr->x, spr->y + h);
    glEnd();
}

void draw_sprite_fast(Tsprite *spr) {
    draw_sprite_fast_generic(spr);
}

void draw_tile16_fast(Tsprite *spr) {
    draw_sprite_fast_generic(spr);
}

void draw_tile8_fast(Tsprite *spr) {
    draw_sprite_fast_generic(spr);
}
