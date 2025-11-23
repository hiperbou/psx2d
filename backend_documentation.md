# Backend Interface Documentation

This document outlines the functions that a new backend for the game engine needs to implement. These functions are the interface between the game logic and the platform-specific code.

## Building a Backend

To compile a new backend, you must create a new `Makefile.<platform>` file. This file tells the build system which source files to include for your specific platform.

1.  **Define Platform Modules:** Create a `MODULES_<PLATFORM>` variable that lists the directories containing your backend's source code.
2.  **Include Default Modules:** You can reuse default implementations for certain systems by including `core/default` in your platform modules list.
3.  **Set Compiler Flags:** Add a platform-specific definition to `CPPFLAGS` (e.g., `-DOPENGL`).
4.  **Set Linker Flags:** Add any required libraries to `LDFLAGS` (e.g., `-lGL -lglut`).

See `Makefile.opengl` for a complete example.

## Asset Loading

A backend is responsible for loading image data from disk or memory and converting it into a platform-specific texture or surface that can be used for rendering.

The engine does not provide a built-in image decoding library (e.g., for PNG, JPG). **A new backend must supply its own image loading solution.** A recommended approach is to use a lightweight, single-header library like `stb_image.h`.

## Default Implementations

The engine provides default, platform-agnostic implementations for certain core functions. A new backend can choose to use these defaults or provide its own custom implementation.

*   **Memory Management (`src/core/default/hgl_mem.c`):** Wrappers for the standard C library (`malloc`, `free`, etc.).
*   **Main Loop (`src/core/default/hgl_main_loop.c`):** A basic main loop for desktop and web.
*   **Engine-level Rendering (`src/engine/sprites.c`):** Default implementations for `initSprites` and `draw_text8`.

## Core Functions (`hgl.h`)

### `HGL_init()`
**Description:** Initializes the backend (graphics, input, etc.).

### `HGL_frame()`
**Description:** Called once per frame to handle per-frame updates.

### `setMainLoopCallback(MainLoopCallback *mainLoop)`
**Description:** Sets the function that will be called repeatedly as the game's main loop.
*   `mainLoop`: A pointer to the function to be called.
*   **Default Implementation Available.**

### `setClearColor(uint8_t r, uint8_t g, uint8_t b)`
**Description:** Sets the screen clear color.
*   `r`, `g`, `b`: The red, green, and blue components of the clear color (0-255).

### `fadeToBlack(uint8_t fade)`
**Description:** Fades the screen to black.
*   `fade`: An alpha value from 0 (no fade) to 255 (fully black).

### `fadeToWhite(uint8_t fade)`
**Description:** Fades the screen to white.
*   `fade`: An alpha value from 0 (no fade) to 255 (fully white).

## Media & File I/O Functions

### `GetSpriteFromDisc(char *filename, SPRITE *sprite)`
**Description:** Loads a sprite from a file into a backend-specific `SPRITE` structure.
*   `filename`: The path to the sprite file.
*   `sprite`: A pointer to a `SPRITE` structure that the backend will fill with the loaded data.

### `GetSpriteFromMemory(uint8_t *data, int len, SPRITE *sprite)`
**Description:** Loads a sprite from a memory buffer into a backend-specific `SPRITE` structure.
*   `data`: A pointer to the in-memory sprite data.
*   `len`: The length of the data buffer in bytes.
*   `sprite`: A pointer to a `SPRITE` structure that the backend will fill with the loaded data.
*   **Note:** The canonical engine API is missing the `len` parameter, which is a bug. A correct implementation requires it.

### `CopySprite(SPRITE *original, SPRITE *sprite)`
**Description:** Copies the contents of one backend-specific `SPRITE` structure to another.
*   `original`: The source `SPRITE` to copy from.
*   `sprite`: The destination `SPRITE` to copy to.

### `SetSpriteDimensions(SPRITE *sprite, float u, float v, float w, float h)`
**Description:** Sets the dimensions of a backend-specific `SPRITE` structure.
*   `sprite`: The `SPRITE` to modify.
*   `u`, `v`: The UV coordinates of the sprite.
*   `w`, `h`: The width and height of the sprite.

## Memory Management Functions (`hgl_mem.h`)

*(Note: These functions have a default implementation.)*

### `HGL_InitHeap(unsigned long *buffer, unsigned long size)`
**Description:** Initializes a custom memory heap from a given buffer.
*   `buffer`: A pointer to the start of the memory block.
*   `size`: The size of the memory block.

### `HGL_free(void *ptr)`
**Description:** Frees a block of memory.
*   `ptr`: A pointer to the memory to free.

### `HGL_malloc(size_t size)`
**Description:** Allocates a block of memory of the specified size.
*   `size`: The number of bytes to allocate.

### `HGL_calloc(size_t numItems, size_t size)`
**Description:** Allocates memory for an array and initializes it to zero.
*   `numItems`: The number of elements to allocate.
*   `size`: The size of each element.

### `HGL_realloc(void *ptr, size_t size)`
**Description:** Changes the size of the memory block pointed to by `ptr`.
*   `ptr`: A pointer to the memory block to resize.
*   `size`: The new size of the memory block.

### `HGL_memcpy(uint8_t *dest, const uint8_t *src, size_t size)`
**Description:** Copies a block of memory.
*   `dest`: The destination buffer.
*   `src`: The source buffer.
*   `size`: The number of bytes to copy.

### `HGL_memset(uint8_t *dest, int value, size_t size)`
**Description:** Fills a block of memory with a specific value.
*   `dest`: The buffer to fill.
*   `value`: The value to set (as an int, but written as a byte).
*   `size`: The number of bytes to set.

## Input Functions (`input.h`)

### `getPadState(int player)`
**Description:** Gets the full state of the gamepad for the specified player.
*   `player`: The player number (0 for player 1, etc.).

### `getButtons(int player)`
**Description:** Gets the state of the digital buttons for the specified player as a bitmask.
*   `player`: The player number (0 for player 1, etc.).

## Rendering Functions (`sprites.h`)

### `initSprites()`
**Description:** Initializes the sprite rendering system.
*   **Default Implementation Available.**

### `draw_sprite(Tsprite *spr)`
**Description:** Draws a single sprite with support for transformations (scaling, rotation, flipping).
*   `spr`: A pointer to the engine-level `Tsprite` to be rendered.

### `draw_sprite_fast(Tsprite *spr)`
**Description:** Draws a single sprite without transformations as a performance optimization.
*   `spr`: A pointer to the engine-level `Tsprite` to be rendered.

### `draw_tile16_fast(Tsprite *spr)`
**Description:** A highly optimized function for drawing 16x16 tiles.
*   `spr`: A pointer to the engine-level `Tsprite` representing the tile.

### `draw_tile8_fast(Tsprite *spr)`
**Description:** A highly optimized function for drawing 8x8 tiles.
*   `spr`: A pointer to the engine-level `Tsprite` representing the tile.

### `draw_text8(int file, int base_map, const char* text, int posX, int posY, int flags, int length)`
**Description:** Draws text using an 8x8 character font map.
*   `file`: The FPG file ID for the font graphics.
*   `base_map`: The map ID within the FPG for the first character of the font.
*   `text`: The string to draw.
*   `posX`, `posY`: The screen position to start drawing.
*   `flags`: Rendering flags (e.g., for alignment).
*   `length`: The number of characters to draw.
*   **Default Implementation Available.**

## Data Structures

### `Tsprite` (Engine-Level)
**Description:** The engine's internal representation of a sprite.
*   `int x, y, z`: The world position of the sprite.
*   `int file`: The FPG file ID for the sprite's graphics.
*   `int graph`: The map ID within the FPG file.
*   `int size_x, size_y`: The width and height of the sprite.
*   `int angle`: The rotation angle of the sprite.
*   `int flags`: Rendering flags (e.g., for flipping).

### `SPRITE` (Backend-Specific)
**Description:** The backend's internal representation of sprite graphics data.

#### Example: PSX Backend
```c
typedef struct _SPRITE {
    u_short tpage; u_short clut; u_char u,v,w,h; CVECTOR col;
} SPRITE;
```

#### Example: ctoy Backend
```c
typedef struct SPRITE { CToyImage m_image; int32_t u,v,w,h; } SPRITE;
```

#### Example: raylib Backend
```c
typedef struct SPRITE { Texture2D m_image; int u,v,w,h; } SPRITE;
```

#### Example: OpenGL Backend
```c
typedef struct SPRITE {
    GLuint texture_id;
    int u, v;         // UV offset in pixels
    int w, h;         // Sprite width/height in pixels
    int tex_w, tex_h; // Full texture width/height for UV calculation
} SPRITE;
```

### `PADSTATE`
**Description:** Represents the complete state of a gamepad.
*   `unsigned short btn`: Bitmask of the currently pressed buttons.
*   `unsigned char rs_x, rs_y`, `ls_x, ls_y`: Positions of the analog sticks.

### Button Bitmasks
The `btn` field uses the following bit flags:
*   `PAD_SELECT`: 1, `PAD_START`: 8
*   `PAD_UP`: 16, `PAD_RIGHT`: 32, `PAD_DOWN`: 64, `PAD_LEFT`: 128
*   `PAD_L2`: 256, `PAD_R2`: 512, `PAD_L1`: 1024, `PAD_R1`: 2048
*   `PAD_TRIANGLE`: 4096, `PAD_CIRCLE`: 8192, `PAD_CROSS`: 16384, `PAD_SQUARE`: 32768

## Implementation Notes

### OpenGL Backend (GLFW)
The OpenGL backend uses GLFW for window management and input. Key implementation details:
*   **Resolution:** Uses a 640x480 window with a 320x240 internal game resolution (2x scaling).
*   **Timing:** Implements a hybrid sleep/busy-wait loop for precise 60 FPS timing. On Windows, it uses `timeBeginPeriod(1)` to ensure 1ms timer resolution.
*   **Texture Loading:** Uses `stb_image.h` to load textures. It automatically appends `.png` to filenames if missing.
*   **UV Coordinates:** For atlas sprites, UVs are normalized using the full texture dimensions stored in `tex_w` and `tex_h`.
*   **Input Mapping:**
    *   `X` Key -> `PAD_CROSS`
    *   `Z` Key -> `PAD_SQUARE`
    *   `Enter` Key -> `PAD_START`
    *   Arrow Keys -> D-Pad
