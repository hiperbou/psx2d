#include <SDL3/SDL.h>

#include "../../core/hgl.h"       // HGL_init, HGL_frame, HGL_quit (expected)
#include "../../game.h"          // Game_init, Game_update, Game_draw, Game_exit
#include "../../input/input.h"     // initInput (may be called by HGL_init or gameInit)

// --- Game state and functions ---
// Stubs for game functions if not provided by actual game code.
// These allow the platform layer to compile and run.
static int game_is_running_flag = 1; // Controls the main loop

// Attempt to declare actual game functions as extern.
// If the game provides these, they will be linked. Otherwise, stubs are used.
// Note: game.h only showed "gameInit", not the others.
// The subtask description implies these names (Game_init, Game_update etc.)
extern void gameInit(void); // From game.h
// extern void Game_init(void); // As per prompt, but game.h has gameInit
extern void Game_update(float delta_time);
extern void Game_draw(void);
extern void Game_exit(void);
// extern int isGameRunning(void); // isGameRunning is usually platform-side or a shared flag

// Stub implementations
void Game_init_stub_impl() {
    SDL_Log("Game_init_stub_impl: Using stub for game initialization.");
    // If gameInit from game.h is available, it should be used.
    // initInput() is called by HGL_init() in sdl3_hgl.c.
}

void Game_update_stub_impl(float delta_time) {
    (void)delta_time; // Unused in stub
    // Minimal update: check for ESC key to quit as an alternative
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    if (keystate[SDL_SCANCODE_ESCAPE]) {
        game_is_running_flag = 0;
        SDL_Log("ESC pressed, game_is_running_flag set to 0.");
    }
}

void Game_draw_stub_impl() {
    // SDL_Log("Game_draw_stub_impl: Using stub for game drawing.");
    // Screen is cleared by startFrame(). If this stub is used, screen will be blank.
}

void Game_exit_stub_impl() {
    SDL_Log("Game_exit_stub_impl: Using stub for game exit.");
}

int isGameRunning_internal() {
    return game_is_running_flag;
}

// Function pointers, initialized to stubs.
// We will try to point Game_init_ptr to gameInit if available.
void (*Game_init_ptr)(void) = Game_init_stub_impl;
void (*Game_update_ptr)(float) = Game_update_stub_impl;
void (*Game_draw_ptr)(void) = Game_draw_stub_impl;
void (*Game_exit_ptr)(void) = Game_exit_stub_impl;
int (*isGameRunning_ptr)(void) = isGameRunning_internal;


int main(int argc, char *argv[]) {
    (void)argc; // Unused
    (void)argv; // Unused

    SDL_Log("Starting HGL application with SDL3 platform.");

    // Try to use actual gameInit from game.h if it's linked
    // The linker will resolve `gameInit` if it exists and is not static.
    // If we can check its address (not straightforward for non-weak symbols without linker tricks),
    // we could conditionally assign. For now, assume if `game.c` is linked, `gameInit` is available.
    // A common practice is to just call `gameInit()` directly if it's expected to be there.
    // The function pointer approach here is to make it robust to `gameInit` not being defined,
    // falling back to a stub.
    // Let's assume `gameInit` is the one from `game.h`.
    // If `Game_init` (from prompt) was a different function, that would need separate handling.
    if (&gameInit != NULL && (void*)gameInit != (void*)Game_init_stub_impl) { // Check if gameInit is not the stub itself
        Game_init_ptr = gameInit;
        SDL_Log("Using actual gameInit() from game.h/game.c");
    } else {
        SDL_Log("Using Game_init_stub_impl as gameInit() not found or is same as stub.");
    }
    // For other functions (Game_update, Game_draw, Game_exit), we assume they are either
    // defined with those exact names and linked, or the stubs will be used.
    // If the game engine provides `Game_update`, `Game_draw`, `Game_exit`, the linker will use them.
    // This requires those functions to be declared `extern` (implicitly or explicitly)
    // and defined globally in the game's object files.
    // For now, if they are not linked, the stubs are the fallback.

    // Initialize HGL (SDL, window, renderer, etc.)
    if (HGL_init() != 0) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "HGL_init() failed. Exiting.");
        return -1;
    }
    // initInput() is called within HGL_init() in sdl3_hgl.c

    // Initialize Game Logic
    Game_init_ptr();

    Uint64 last_time = SDL_GetPerformanceCounter();
    double delta_time_ms = 0;

    SDL_Log("Entering main game loop...");
    while (isGameRunning_ptr()) { // Uses internal flag, modified by events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                SDL_Log("SDL_EVENT_QUIT received, setting game_is_running_flag to 0.");
                game_is_running_flag = 0;
            }
            // TODO: Pass event to a more sophisticated input handler if necessary
            // e.g., if getButtons() in sdl3_input.c is not sufficient.
            // Or for things like text input if that gets added.
        }

        Uint64 current_time = SDL_GetPerformanceCounter();
        delta_time_ms = (double)((current_time - last_time) * 1000.0 / SDL_GetPerformanceFrequency());
        last_time = current_time;

        // Update game logic (pass delta_time in seconds)
        Game_update_ptr((float)(delta_time_ms / 1000.0));

        // Drawing:
        startFrame();    // Clears the screen and prepares for rendering (from hgl.h -> sdl3_hgl.c)
        Game_draw_ptr(); // Contains all game-specific drawing calls
        HGL_frame();     // Presents the current frame (from hgl.h -> sdl3_hgl.c)
    }

    SDL_Log("Exiting main game loop.");

    // Cleanup game resources
    Game_exit_ptr();

    // Cleanup HGL (SDL resources)
    // HGL_quit(); // This should call SDL_DestroyRenderer, SDL_DestroyWindow, SDL_QuitSubSystem, SDL_Quit

    // If HGL_quit() is not available or doesn't call SDL_Quit():
    // SDL_DestroyRenderer(renderer); // renderer is not visible here
    // SDL_DestroyWindow(window);   // window is not visible here
    SDL_Quit(); // Fallback if HGL_quit doesn't exist or is incomplete.
                // My `sdl3_hgl.c` has HGL_quit() which calls SDL_DestroyRenderer, SDL_DestroyWindow, SDL_Quit.
                // So, calling HGL_quit() is preferred if available.
                // For now, directly calling SDL_Quit() as HGL_quit() is not in hgl.h.
                // It *should* be in hgl.h if it's meant to be the public API for shutdown.

    SDL_Log("Application finished.");
    return 0;
}

// Note: To make this robust, sdl3_hgl.c should be revisited:
// 1. HGL_init() should return int status. Main should check it.
// 2. HGL_frame() should definitively either include startFrame() or startFrame() should be public via hgl.h.
//    The prompt for sdl3_hgl.c implied HGL_frame would call startFrame.
// 3. HGL_quit() should be declared in hgl.h and called from main.
// For the stubs:
// game.h needs to declare Game_init, Game_update, Game_draw, Game_exit, isGameRunning.
// Or, the build system ensures GAME_LOGIC_DEFINED is set and these are linked.
// The current game.h only shows gameInit().
// This main.c uses Game_init_ptr which defaults to Game_init_stub if Game_init is not found.
// If the actual game uses `gameInit`, then `Game_init_ptr = gameInit;` would be needed after checking its availability.
// Or, `game.h` should be updated to use `Game_init` to match the prompt's convention for this file.
// For now, the stubs provide a runnable, if non-functional, shell.The file `src/platform/sdl3/sdl3_main.c` has been created.
However, I must address the discrepancies noted in the previous "Thinking" step regarding `startFrame()` and `HGL_quit()`, and the game function names.

**Plan:**
1.  **Modify `src/core/hgl.h`**:
    *   Add declaration for `void HGL_quit(void);`
    *   Add declaration for `void startFrame(void);`
    *   Modify `HGL_init` to return an `int` status.
2.  **Modify `src/core/sdl3/sdl3_hgl.c`**:
    *   Ensure `HGL_init` returns 0 on success, -1 on failure.
    *   Ensure `HGL_frame` only calls `SDL_RenderPresent(renderer);` (as it currently does). `startFrame()` will be called explicitly from `main`.
    *   Make `startFrame` non-static so it can be called from `main.c` (it's declared in `hgl.h`).
    *   Ensure `HGL_quit` is implemented correctly.
3.  **Modify `src/platform/sdl3/sdl3_main.c`**:
    *   Call `startFrame()` before `Game_draw_ptr()`.
    *   Call `HGL_quit()` before returning from `main`.
    *   Check the return value of `HGL_init()`.
    *   Adjust the game function pointers: The prompt uses `Game_init`, `Game_update` etc., while the existing `game.h` has `gameInit`. I will use the `Game_xxx_ptr` to point to `gameInit` if `Game_init` is not found, assuming `gameInit` is the intended entry point for initialization. For other functions (`Game_update`, `Game_draw`, `Game_exit`, `isGameRunning`), stubs will be used if they are not defined and linked from the actual game code.

Let's start by modifying `src/core/hgl.h`.
