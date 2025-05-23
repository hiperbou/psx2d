#include <SDL3/SDL.h>
#include "../../input/input.h" // Relative path to input.h from src/input/sdl3/

// Static variable to hold the gamepad instance for player 0
static SDL_Gamepad *gamepad_player0 = NULL;

void initInput() {
    // Initialize SDL's gamepad subsystem if not already initialized
    // SDL_InitSubSystem(SDL_INIT_EVENTS) is usually covered by general SDL_Init
    if (!SDL_WasInit(SDL_INIT_GAMEPAD)) {
        if (SDL_InitSubSystem(SDL_INIT_GAMEPAD) < 0) {
            SDL_Log("Failed to initialize SDL Gamepad subsystem: %s", SDL_GetError());
            return;
        }
    }

    // Check for available gamepads and open the first one for player 0
    // SDL_UpdateJoysticks(); // SDL3 uses SDL_UpdateGamepads() or automatic updates
    SDL_UpdateGamepads(); 
    
    int num_joysticks = SDL_GetNumJoysticks();
    if (num_joysticks > 0) {
        if (SDL_IsGamepad(0)) { // Check if joystick at index 0 is a gamepad
            gamepad_player0 = SDL_OpenGamepad(0);
            if (gamepad_player0) {
                SDL_Log("Gamepad 0 opened: %s", SDL_GetGamepadName(gamepad_player0));
            } else {
                SDL_Log("Could not open gamepad 0: %s", SDL_GetError());
            }
        } else {
             SDL_Log("Joystick 0 is not a recognized gamepad.");
        }
    } else {
        SDL_Log("No joysticks/gamepads found.");
    }
}

// Optional: Function to close the gamepad, e.g., on game exit
void quitInput() {
    if (gamepad_player0) {
        SDL_CloseGamepad(gamepad_player0);
        gamepad_player0 = NULL;
        SDL_Log("Gamepad 0 closed.");
    }
    // If SDL_InitSubSystem(SDL_INIT_GAMEPAD) was called, 
    // SDL_QuitSubSystem(SDL_INIT_GAMEPAD) can be called on full shutdown.
    // However, SDL_Quit() handles this.
}

PADSTATE *getPadState(int player) {
    // For now, this function returns NULL as per the plan.
    // It can be expanded later to provide more detailed analog and pressure data if needed.
    if (player == 0 && gamepad_player0) {
        // If we wanted to return a static PADSTATE, we could populate it here.
        // static PADSTATE p_state;
        // p_state.btn = getButtons(0); // Example
        // p_state.ls_x = (SDL_GetGamepadAxis(gamepad_player0, SDL_GAMEPAD_AXIS_LEFTX) / 256) + 128;
        // ... etc.
        // return &p_state;
    }
    return NULL;
}

uint16_t getButtons(int player) {
    uint16_t buttons = 0;
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);

    // For now, only handle player 0 for keyboard and gamepad
    if (player != 0) {
        return 0;
    }

    // Keyboard input
    // Exclusive left/right (pressing both cancels out)
    if (keystate[SDL_SCANCODE_LEFT] && !keystate[SDL_SCANCODE_RIGHT]) {
        buttons |= PAD_LEFT;
    }
    if (keystate[SDL_SCANCODE_RIGHT] && !keystate[SDL_SCANCODE_LEFT]) {
        buttons |= PAD_RIGHT;
    }
    // Non-exclusive up/down (can press both, though game logic might handle it)
    if (keystate[SDL_SCANCODE_UP]) {
        buttons |= PAD_UP;
    }
    if (keystate[SDL_SCANCODE_DOWN]) {
        buttons |= PAD_DOWN;
    }

    if (keystate[SDL_SCANCODE_X]) { // Mapped to Cross
        buttons |= PAD_CROSS;
    }
    if (keystate[SDL_SCANCODE_Z]) { // Mapped to Square
        buttons |= PAD_SQUARE;
    }
    if (keystate[SDL_SCANCODE_A]) { // Mapped to Circle
        buttons |= PAD_CIRCLE;
    }
    if (keystate[SDL_SCANCODE_S]) { // Mapped to Triangle
        buttons |= PAD_TRIANGLE;
    }
    if (keystate[SDL_SCANCODE_RETURN] || keystate[SDL_SCANCODE_RETURN2]) {
        buttons |= PAD_START;
    }
    if (keystate[SDL_SCANCODE_BACKSPACE]) { // Example for Select
        buttons |= PAD_SELECT;
    }
    
    // Gamepad input (if available for player 0)
    if (gamepad_player0) {
        if (SDL_GetGamepadButton(gamepad_player0, SDL_GAMEPAD_BUTTON_DPAD_UP)) {
            buttons |= PAD_UP;
        }
        if (SDL_GetGamepadButton(gamepad_player0, SDL_GAMEPAD_BUTTON_DPAD_DOWN)) {
            buttons |= PAD_DOWN;
        }
        if (SDL_GetGamepadButton(gamepad_player0, SDL_GAMEPAD_BUTTON_DPAD_LEFT)) {
            buttons |= PAD_LEFT;
        }
        if (SDL_GetGamepadButton(gamepad_player0, SDL_GAMEPAD_BUTTON_DPAD_RIGHT)) {
            buttons |= PAD_RIGHT;
        }

        // Standard SDL Gamepad Button Mappings (often based on Xbox layout)
        // A -> PAD_CROSS (common for confirmation)
        // B -> PAD_CIRCLE (common for cancel)
        // X -> PAD_SQUARE
        // Y -> PAD_TRIANGLE
        if (SDL_GetGamepadButton(gamepad_player0, SDL_GAMEPAD_BUTTON_A)) { // Typically 'A' on Xbox, Cross on PlayStation
            buttons |= PAD_CROSS;
        }
        if (SDL_GetGamepadButton(gamepad_player0, SDL_GAMEPAD_BUTTON_B)) { // Typically 'B' on Xbox, Circle on PlayStation
            buttons |= PAD_CIRCLE;
        }
        if (SDL_GetGamepadButton(gamepad_player0, SDL_GAMEPAD_BUTTON_X)) { // Typically 'X' on Xbox, Square on PlayStation
            buttons |= PAD_SQUARE;
        }
        if (SDL_GetGamepadButton(gamepad_player0, SDL_GAMEPAD_BUTTON_Y)) { // Typically 'Y' on Xbox, Triangle on PlayStation
            buttons |= PAD_TRIANGLE;
        }
        if (SDL_GetGamepadButton(gamepad_player0, SDL_GAMEPAD_BUTTON_START)) {
            buttons |= PAD_START;
        }
        if (SDL_GetGamepadButton(gamepad_player0, SDL_GAMEPAD_BUTTON_BACK)) { // 'Back' or 'Select' button
            buttons |= PAD_SELECT;
        }
        if (SDL_GetGamepadButton(gamepad_player0, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER)) {
            buttons |= PAD_L1;
        }
        if (SDL_GetGamepadButton(gamepad_player0, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER)) {
            buttons |= PAD_R1;
        }
        // SDL_GAMEPAD_BUTTON_LEFT_STICK, SDL_GAMEPAD_BUTTON_RIGHT_STICK for L3/R3
        if (SDL_GetGamepadButton(gamepad_player0, SDL_GAMEPAD_BUTTON_LEFT_STICK)) {
            buttons |= PAD_L3;
        }
        if (SDL_GetGamepadButton(gamepad_player0, SDL_GAMEPAD_BUTTON_RIGHT_STICK)) {
            buttons |= PAD_R3;
        }

        // Analog triggers (L2/R2) - treat as digital for simplicity
        // SDL_GAMEPAD_AXIS_LEFT_TRIGGER, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER
        // These are axes from 0 to SDL_JOYSTICK_AXIS_MAX (32767)
        // A simple threshold can be used.
        if (SDL_GetGamepadAxis(gamepad_player0, SDL_GAMEPAD_AXIS_LEFT_TRIGGER) > 16000) { // Arbitrary threshold
             buttons |= PAD_L2;
        }
        if (SDL_GetGamepadAxis(gamepad_player0, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) > 16000) { // Arbitrary threshold
             buttons |= PAD_R2;
        }
    }

    return buttons;
}
