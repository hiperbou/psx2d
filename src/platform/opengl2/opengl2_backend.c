
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "opengl2_backend.h"
#include "../../engine/sprites.h" // For Tsprite
#include "../../input/input.h" // For PADSTATE
#include "../../media/fpg.h" // For fpg array

#ifdef _WIN32
    #include <windows.h>
#else
    #include <time.h>
#endif

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../../external/stb_image.h"

#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_ARRAY_BUFFER 0x8892
#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_INFO_LOG_LENGTH 0x8B84

typedef ptrdiff_t GLsizeiptr;
typedef char GLchar;

typedef GLuint (APIENTRY *PFNGLCREATESHADERPROC) (GLenum type);
typedef void (APIENTRY *PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void (APIENTRY *PFNGLCOMPILESHADERPROC) (GLuint shader);
typedef void (APIENTRY *PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint *params);
typedef void (APIENTRY *PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef GLuint (APIENTRY *PFNGLCREATEPROGRAMPROC) (void);
typedef void (APIENTRY *PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (APIENTRY *PFNGLLINKPROGRAMPROC) (GLuint program);
typedef void (APIENTRY *PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint *params);
typedef void (APIENTRY *PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRY *PFNGLUSEPROGRAMPROC) (GLuint program);
typedef GLint (APIENTRY *PFNGLGETATTRIBLOCATIONPROC) (GLuint program, const GLchar *name);
typedef GLint (APIENTRY *PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const GLchar *name);
typedef void (APIENTRY *PFNGLUNIFORM1IPROC) (GLint location, GLint v0);
typedef void (APIENTRY *PFNGLUNIFORM4FPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (APIENTRY *PFNGLUNIFORMMATRIX4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRY *PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRY *PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY *PFNGLBUFFERDATAPROC) (GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (APIENTRY *PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef void (APIENTRY *PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);

static PFNGLCREATESHADERPROC glCreateShader;
static PFNGLSHADERSOURCEPROC glShaderSource;
static PFNGLCOMPILESHADERPROC glCompileShader;
static PFNGLGETSHADERIVPROC glGetShaderiv;
static PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
static PFNGLCREATEPROGRAMPROC glCreateProgram;
static PFNGLATTACHSHADERPROC glAttachShader;
static PFNGLLINKPROGRAMPROC glLinkProgram;
static PFNGLGETPROGRAMIVPROC glGetProgramiv;
static PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
static PFNGLUSEPROGRAMPROC glUseProgram;
static PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
static PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
static PFNGLUNIFORM1IPROC glUniform1i;
static PFNGLUNIFORM4FPROC glUniform4f;
static PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
static PFNGLGENBUFFERSPROC glGenBuffers;
static PFNGLBINDBUFFERPROC glBindBuffer;
static PFNGLBUFFERDATAPROC glBufferData;
static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
static PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;

void loadGLFunctions() {
    glCreateShader = (PFNGLCREATESHADERPROC)glfwGetProcAddress("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)glfwGetProcAddress("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)glfwGetProcAddress("glCompileShader");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)glfwGetProcAddress("glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)glfwGetProcAddress("glGetShaderInfoLog");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)glfwGetProcAddress("glCreateProgram");
    glAttachShader = (PFNGLATTACHSHADERPROC)glfwGetProcAddress("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)glfwGetProcAddress("glLinkProgram");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)glfwGetProcAddress("glGetProgramiv");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)glfwGetProcAddress("glGetProgramInfoLog");
    glUseProgram = (PFNGLUSEPROGRAMPROC)glfwGetProcAddress("glUseProgram");
    glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)glfwGetProcAddress("glGetAttribLocation");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)glfwGetProcAddress("glGetUniformLocation");
    glUniform1i = (PFNGLUNIFORM1IPROC)glfwGetProcAddress("glUniform1i");
    glUniform4f = (PFNGLUNIFORM4FPROC)glfwGetProcAddress("glUniform4f");
    glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)glfwGetProcAddress("glUniformMatrix4fv");
    glGenBuffers = (PFNGLGENBUFFERSPROC)glfwGetProcAddress("glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)glfwGetProcAddress("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)glfwGetProcAddress("glBufferData");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glfwGetProcAddress("glEnableVertexAttribArray");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)glfwGetProcAddress("glVertexAttribPointer");
}

static GLFWwindow* window = NULL;
static const int gameScreenWidth = 320;
static const int gameScreenHeight = 240;
static const int windowWidth = 640;
static const int windowHeight = 480;

static const double targetFPS = 60.0;
static const double targetFrameTime = 1.0 / 60.0;
static double lastFrameTime = 0.0;

static GLuint shaderProgram;
static GLuint vbo;
static GLint uProjection;
static GLint uTexture;
static GLint uTexBounds;
static GLuint whiteTexture;

typedef struct {
    float x, y;
    float u, v;
    float r, g, b, a;
} Vertex;

const char* vertexShaderSrc = 
    "attribute vec2 aPos;"
    "attribute vec2 aTexCoord;"
    "attribute vec4 aColor;"
    "varying vec2 vTexCoord;"
    "varying vec4 vColor;"
    "uniform mat4 uProjection;"
    "void main() {"
    "    gl_Position = uProjection * vec4(aPos, 0.0, 1.0);"
    "    vTexCoord = aTexCoord;"
    "    vColor = aColor;"
    "}";

const char* fragmentShaderSrc = 
    "varying vec2 vTexCoord;"
    "varying vec4 vColor;"
    "uniform sampler2D uTexture;"
    "uniform vec4 uTexBounds;"
    "void main() {"
    "    vec2 clampedUV = clamp(vTexCoord, uTexBounds.xy, uTexBounds.zw);"
    "    gl_FragColor = texture2D(uTexture, clampedUV) * vColor;"
    "}";

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Shader Compilation Error: %s\n", infoLog);
    }
    return shader;
}

void initShaders() {
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
    
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);
    
    glUseProgram(shaderProgram);
    uProjection = glGetUniformLocation(shaderProgram, "uProjection");
    uTexture = glGetUniformLocation(shaderProgram, "uTexture");
    uTexBounds = glGetUniformLocation(shaderProgram, "uTexBounds");
    
    float L = 0.0f, R = 320.0f, B = 240.0f, T = 0.0f;
    float ortho[16] = {
        2.0f/(R-L), 0.0f,        0.0f, 0.0f,
        0.0f,       2.0f/(T-B),  0.0f, 0.0f,
        0.0f,       0.0f,       -1.0f, 0.0f,
        -(R+L)/(R-L), -(T+B)/(T-B), 0.0f, 1.0f
    };
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, ortho);
    glUniform1i(uTexture, 0);
    
    unsigned char whitePixel[4] = {255, 255, 255, 255};
    glGenTextures(1, &whiteTexture);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
}

static int vpX = 0, vpY = 0, vpW = 0, vpH = 0;
static uint8_t clearR = 0, clearG = 0, clearB = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    float targetAspect = (float)gameScreenWidth / (float)gameScreenHeight;
    float windowAspect = (float)width / (float)height;
    
    if (windowAspect > targetAspect) {
        vpH = height;
        vpW = (int)(height * targetAspect);
        vpX = (width - vpW) / 2;
        vpY = 0;
    } else {
        vpW = width;
        vpH = (int)(width / targetAspect);
        vpX = 0;
        vpY = (height - vpH) / 2;
    }
    
    glViewport(vpX, vpY, vpW, vpH);
}

void HGL_init() {
    if (!glfwInit()) return;

    window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL 2 Backend", NULL, NULL);
    if (!window) { glfwTerminate(); return; }

    glfwMakeContextCurrent(window);
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    framebuffer_size_callback(window, w, h);
    
    loadGLFunctions();
    
    initShaders();
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    GLint locPos = glGetAttribLocation(shaderProgram, "aPos");
    GLint locTex = glGetAttribLocation(shaderProgram, "aTexCoord");
    GLint locCol = glGetAttribLocation(shaderProgram, "aColor");
    
    glEnableVertexAttribArray(locPos);
    glEnableVertexAttribArray(locTex);
    glEnableVertexAttribArray(locCol);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
    if (frameTime < targetFrameTime) {
        double waitTime = targetFrameTime - frameTime;
        double sleepTime = waitTime * 0.95;
        #ifdef _WIN32
            if (sleepTime > 0.001) Sleep((DWORD)(sleepTime * 1000.0));
        #else
            if (sleepTime > 0.000001) {
                struct timespec req = { (time_t)sleepTime, (long)((sleepTime - (time_t)sleepTime) * 1000000000L) };
                nanosleep(&req, NULL);
            }
        #endif
        while (glfwGetTime() < currentTime + waitTime) {}
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

void draw_quad_color(float r, float g, float b, float a) {
    Vertex verts[6] = {
        {0, 0, 0, 0, r, g, b, a}, {gameScreenWidth, 0, 0, 0, r, g, b, a}, {gameScreenWidth, gameScreenHeight, 0, 0, r, g, b, a},
        {0, 0, 0, 0, r, g, b, a}, {gameScreenWidth, gameScreenHeight, 0, 0, r, g, b, a}, {0, gameScreenHeight, 0, 0, r, g, b, a}
    };

    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    glUniform4f(uTexBounds, 0.0f, 0.0f, 1.0f, 1.0f);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
    
    GLint locPos = glGetAttribLocation(shaderProgram, "aPos");
    GLint locTex = glGetAttribLocation(shaderProgram, "aTexCoord");
    GLint locCol = glGetAttribLocation(shaderProgram, "aColor");
    
    glVertexAttribPointer(locPos, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribPointer(locTex, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(float)));
    glVertexAttribPointer(locCol, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(4*sizeof(float)));
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void fadeToBlack(uint8_t fade) {
    draw_quad_color(0.0f, 0.0f, 0.0f, fade / 255.0f);
}

void fadeToWhite(uint8_t fade) {
    draw_quad_color(1.0f, 1.0f, 1.0f, fade / 255.0f);
}

void GetSpriteFromDisc(char *filename, SPRITE *sprite) {
    int width, height, channels;
    char fullFilename[256];
    snprintf(fullFilename, sizeof(fullFilename), "%s.png", filename);
    
    unsigned char *data = stbi_load(fullFilename, &width, &height, &channels, 4);
    if (!data) return;

    glGenTextures(1, &sprite->texture_id);
    glBindTexture(GL_TEXTURE_2D, sprite->texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    sprite->u = 0; sprite->v = 0;
    sprite->w = width; sprite->h = height;
    sprite->tex_w = width; sprite->tex_h = height;

    // Calculate and cache texture bounds with 0.5 texel inset
    float texW = (float)width;
    float texH = (float)height;
    sprite->minU = ((float)sprite->u + 0.5f) / texW;
    sprite->minV = ((float)sprite->v + 0.5f) / texH;
    sprite->maxU = ((float)(sprite->u + sprite->w) - 0.5f) / texW;
    sprite->maxV = ((float)(sprite->v + sprite->h) - 0.5f) / texH;
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

    sprite->u = 0; sprite->v = 0;
    sprite->w = width; sprite->h = height;
    sprite->tex_w = width; sprite->tex_h = height;
    */
}

void CopySprite(SPRITE *original, SPRITE *sprite) {
    *sprite = *original;
}

void SetSpriteDimensions(SPRITE *sprite, float u, float v, float w, float h) {
    sprite->u = u;
    sprite->v = v;
    sprite->w = w;
    sprite->h = h;
    // Calculate and cache texture bounds with 0.5 texel inset
    float texW = (float)sprite->tex_w;
    float texH = (float)sprite->tex_h;
    sprite->minU = ((float)sprite->u + 0.5f) / texW;
    sprite->minV = ((float)sprite->v + 0.5f) / texH;
    sprite->maxU = ((float)(sprite->u + sprite->w) - 0.5f) / texW;
    sprite->maxV = ((float)(sprite->v + sprite->h) - 0.5f) / texH;
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

void initInput() { glfwSetKeyCallback(window, key_callback); }
PADSTATE* getPadState(int player) { pad_state.btn = button_state; return &pad_state; }
uint16_t getButtons(int player) { return (player == 0) ? button_state : 0; }


void draw_sprite(Tsprite *spr) {
    if (spr == NULL || spr->graph <= 0 || spr->id < 0) return;
    SPRITE* sprite = fpg[spr->file]->map[spr->graph]->image;
    if (sprite == NULL) return;

    float x = spr->x;
    float y = spr->y;
    float scale = spr->size_x / 4096.0f;
    float w = sprite->w * scale;
    float h = sprite->h * scale;
    float angle = (spr->angle * 360.0f) / 4096.0f;

    float u = (float)sprite->u / sprite->tex_w;
    float v = (float)sprite->v / sprite->tex_h;
    float u2 = (float)(sprite->u + sprite->w) / sprite->tex_w;
    float v2 = (float)(sprite->v + sprite->h) / sprite->tex_h;

    if (spr->flags & 1) { float t=u; u=u2; u2=t; }
    if (spr->flags & 2) { float t=v; v=v2; v2=t; }

    float x0 = -w/2, y0 = -h/2;
    float x1 = w/2,  y1 = -h/2;
    float x2 = w/2,  y2 = h/2;
    float x3 = -w/2, y3 = h/2;

    if (angle != 0) {
        float rad = angle * 3.14159f / 180.0f;
        float c = cosf(rad), s = sinf(rad);
        float tx, ty;
        
        tx = x0*c - y0*s; ty = x0*s + y0*c; x0=tx; y0=ty;
        tx = x1*c - y1*s; ty = x1*s + y1*c; x1=tx; y1=ty;
        tx = x2*c - y2*s; ty = x2*s + y2*c; x2=tx; y2=ty;
        tx = x3*c - y3*s; ty = x3*s + y3*c; x3=tx; y3=ty;
    }

    x0 += x; y0 += y;
    x1 += x; y1 += y;
    x2 += x; y2 += y;
    x3 += x; y3 += y;

    Vertex verts[6] = {
        {x0, y0, u, v, 1.0f, 1.0f, 1.0f, 1.0f}, {x1, y1, u2, v, 1.0f, 1.0f, 1.0f, 1.0f}, {x2, y2, u2, v2, 1.0f, 1.0f, 1.0f, 1.0f}, // Tri 1
        {x0, y0, u, v, 1.0f, 1.0f, 1.0f, 1.0f}, {x2, y2, u2, v2, 1.0f, 1.0f, 1.0f, 1.0f}, {x3, y3, u, v2, 1.0f, 1.0f, 1.0f, 1.0f}  // Tri 2
    };

    glBindTexture(GL_TEXTURE_2D, sprite->texture_id);
    glUniform4f(uTexBounds, sprite->minU, sprite->minV, sprite->maxU, sprite->maxV);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
    
    GLint locPos = glGetAttribLocation(shaderProgram, "aPos");
    GLint locTex = glGetAttribLocation(shaderProgram, "aTexCoord");
    GLint locCol = glGetAttribLocation(shaderProgram, "aColor");
    
    glVertexAttribPointer(locPos, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribPointer(locTex, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(float)));
    glVertexAttribPointer(locCol, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(4*sizeof(float)));
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

inline static void draw_sprite_fast_generic(Tsprite *spr) {
    if (spr == NULL || spr->graph <= 0 || spr->id < 0) return;
    SPRITE* sprite = fpg[spr->file]->map[spr->graph]->image;
    if (sprite == NULL) return;

    float x = spr->x;
    float y = spr->y;
    float w = sprite->w;
    float h = sprite->h;

    float u = (float)sprite->u / sprite->tex_w;
    float v = (float)sprite->v / sprite->tex_h;
    float u2 = (float)(sprite->u + sprite->w) / sprite->tex_w;
    float v2 = (float)(sprite->v + sprite->h) / sprite->tex_h;

    if (spr->flags & 1) { float t=u; u=u2; u2=t; }
    if (spr->flags & 2) { float t=v; v=v2; v2=t; }

    float x0 = x;
    float y0 = y;
    float x1 = x + w;
    float y1 = y;
    float x2 = x + w;
    float y2 = y + h;
    float x3 = x;
    float y3 = y + h;

    Vertex verts[6] = {
        {x0, y0, u, v, 1.0f, 1.0f, 1.0f, 1.0f}, {x1, y1, u2, v, 1.0f, 1.0f, 1.0f, 1.0f}, {x2, y2, u2, v2, 1.0f, 1.0f, 1.0f, 1.0f}, // Tri 1
        {x0, y0, u, v, 1.0f, 1.0f, 1.0f, 1.0f}, {x2, y2, u2, v2, 1.0f, 1.0f, 1.0f, 1.0f}, {x3, y3, u, v2, 1.0f, 1.0f, 1.0f, 1.0f}  // Tri 2
    };

    glBindTexture(GL_TEXTURE_2D, sprite->texture_id);
    glUniform4f(uTexBounds, sprite->minU, sprite->minV, sprite->maxU, sprite->maxV);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
    
    GLint locPos = glGetAttribLocation(shaderProgram, "aPos");
    GLint locTex = glGetAttribLocation(shaderProgram, "aTexCoord");
    GLint locCol = glGetAttribLocation(shaderProgram, "aColor");
    
    glVertexAttribPointer(locPos, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribPointer(locTex, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(float)));
    glVertexAttribPointer(locCol, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(4*sizeof(float)));
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void draw_sprite_fast(Tsprite *spr) { draw_sprite_fast_generic(spr); }
void draw_tile16_fast(Tsprite *spr) { draw_sprite_fast_generic(spr); }
void draw_tile8_fast(Tsprite *spr) { draw_sprite_fast_generic(spr); }
