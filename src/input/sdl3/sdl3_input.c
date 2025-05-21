#include <SDL3/SDL.h>
#include <stdlib.h>  // For exit()
#include "../input.h" // For PADSTATE and PAD_ constants
#include <string.h>  // For memset

#define MAX_PLAYERS 4

static SDL_Gamepad* gamepads[MAX_PLAYERS];
static PADSTATE padStates[MAX_PLAYERS];

void initInput() {
    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD) < 0) {
        SDL_Log("SDL Joystick/Gamepad subsystem could not be initialized! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    memset(gamepads, 0, sizeof(gamepads)); // Initialize all gamepad pointers to NULL
    memset(padStates, 0, sizeof(padStates)); // Clear pad states

    int num_joysticks = 0;
    SDL_JoystickID *joysticks = SDL_GetJoysticks(&num_joysticks);

    int current_gamepad = 0;
    if (joysticks) {
        for (int i = 0; i < num_joysticks && current_gamepad < MAX_PLAYERS; ++i) {
            if (SDL_IsGamepad(joysticks[i])) {
                gamepads[current_gamepad] = SDL_OpenGamepad(joysticks[i]);
                if (gamepads[current_gamepad]) {
                    SDL_Log("Opened Gamepad %d: %s", current_gamepad, SDL_GetGamepadName(gamepads[current_gamepad]));
                    current_gamepad++;
                } else {
                    SDL_Log("Could not open gamepad %d: %s", i, SDL_GetError());
                }
            }
        }
        SDL_free(joysticks);
    }
    SDL_Log("Input initialized, %d gamepads found and opened.", current_gamepad);
}

uint16_t getButtons(int player) {
    if (player < 0 || player >= MAX_PLAYERS) {
        return 0;
    }

    uint16_t buttons = 0;
    // SDL_PumpEvents(); // Update events, including joystick/gamepad states - Replaced by SDL_PollEvent loop

    SDL_Event event;
    // Poll for events to handle SDL_EVENT_QUIT
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            exit(0); // Terminate the game
        }
        // Potentially handle other events like SDL_EVENT_GAMEPAD_ADDED, SDL_EVENT_GAMEPAD_REMOVED here
        // For now, only handling QUIT as per the task.
    }

    // Keyboard input (Player 0 primarily)
    if (player == 0) {
        const Uint8* kbState = SDL_GetKeyboardState(NULL);

        if (kbState[SDL_SCANCODE_UP]) buttons |= PAD_UP;
        if (kbState[SDL_SCANCODE_DOWN]) buttons |= PAD_DOWN;
        if (kbState[SDL_SCANCODE_LEFT]) buttons |= PAD_LEFT;
        if (kbState[SDL_SCANCODE_RIGHT]) buttons |= PAD_RIGHT;

        // Common mapping: A/S/D/W or Z/X/C/V for face buttons
        if (kbState[SDL_SCANCODE_X] || kbState[SDL_SCANCODE_A]) buttons |= PAD_CROSS;    // X or A for Cross/South
        if (kbState[SDL_SCANCODE_C] || kbState[SDL_SCANCODE_S]) buttons |= PAD_CIRCLE;   // C or S for Circle/East
        if (kbState[SDL_SCANCODE_Z] || kbState[SDL_SCANCODE_Q]) buttons |= PAD_SQUARE;   // Z or Q for Square/West (QWERTY Q)
        if (kbState[SDL_SCANCODE_V] || kbState[SDL_SCANCODE_W]) buttons |= PAD_TRIANGLE; // V or W for Triangle/North

        if (kbState[SDL_SCANCODE_RETURN]) buttons |= PAD_START;
        if (kbState[SDL_SCANCODE_RSHIFT] || kbState[SDL_SCANCODE_LSHIFT]) buttons |= PAD_SELECT;
        
        if (kbState[SDL_SCANCODE_PAGEUP] || kbState[SDL_SCANCODE_E]) buttons |= PAD_R1; // PageUp or E for R1
        if (kbState[SDL_SCANCODE_PAGEDOWN] || kbState[SDL_SCANCODE_R]) buttons |= PAD_R2; // PageDown or R for R2 (as button)
        if (kbState[SDL_SCANCODE_HOME] || kbState[SDL_SCANCODE_F]) buttons |= PAD_L1;   // Home or F for L1
        if (kbState[SDL_SCANCODE_END] || kbState[SDL_SCANCODE_G]) buttons |= PAD_L2;   // End or G for L2 (as button)
    }

    // Gamepad input
    if (gamepads[player]) {
        if (SDL_GetGamepadButton(gamepads[player], SDL_GAMEPAD_BUTTON_SOUTH)) buttons |= PAD_CROSS;
        if (SDL_GetGamepadButton(gamepads[player], SDL_GAMEPAD_BUTTON_EAST)) buttons |= PAD_CIRCLE;
        if (SDL_GetGamepadButton(gamepads[player], SDL_GAMEPAD_BUTTON_WEST)) buttons |= PAD_SQUARE;
        if (SDL_GetGamepadButton(gamepads[player], SDL_GAMEPAD_BUTTON_NORTH)) buttons |= PAD_TRIANGLE;

        if (SDL_GetGamepadButton(gamepads[player], SDL_GAMEPAD_BUTTON_DPAD_UP)) buttons |= PAD_UP;
        if (SDL_GetGamepadButton(gamepads[player], SDL_GAMEPAD_BUTTON_DPAD_DOWN)) buttons |= PAD_DOWN;
        if (SDL_GetGamepadButton(gamepads[player], SDL_GAMEPAD_BUTTON_DPAD_LEFT)) buttons |= PAD_LEFT;
        if (SDL_GetGamepadButton(gamepads[player], SDL_GAMEPAD_BUTTON_DPAD_RIGHT)) buttons |= PAD_RIGHT;

        if (SDL_GetGamepadButton(gamepads[player], SDL_GAMEPAD_BUTTON_START)) buttons |= PAD_START;
        if (SDL_GetGamepadButton(gamepads[player], SDL_GAMEPAD_BUTTON_BACK)) buttons |= PAD_SELECT;

        if (SDL_GetGamepadButton(gamepads[player], SDL_GAMEPAD_BUTTON_LEFT_SHOULDER)) buttons |= PAD_L1;
        if (SDL_GetGamepadButton(gamepads[player], SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER)) buttons |= PAD_R1;
        
        if (SDL_GetGamepadButton(gamepads[player], SDL_GAMEPAD_BUTTON_LEFT_STICK)) buttons |= PAD_L3;
        if (SDL_GetGamepadButton(gamepads[player], SDL_GAMEPAD_BUTTON_RIGHT_STICK)) buttons |= PAD_R3;

        // Triggers as buttons (thresholding)
        // SDL_GAMEPAD_AXIS_MIN is -32768, SDL_GAMEPAD_AXIS_MAX is 32767
        // A common threshold for "pressed" is > 30% of max range, e.g. 10000
        // The original request had > 16384
        if (SDL_GetGamepadAxis(gamepads[player], SDL_GAMEPAD_AXIS_LEFT_TRIGGER) > 16384) buttons |= PAD_L2;
        if (SDL_GetGamepadAxis(gamepads[player], SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) > 16384) buttons |= PAD_R2;
    }
    return buttons;
}

PADSTATE * getPadState(int player) {
    if (player < 0 || player >= MAX_PLAYERS) {
        return NULL;
    }
    
    // It's crucial to update SDL events before querying states.
    // SDL_PumpEvents() is called in getButtons, which is good.
    // If getButtons() was not called first, it should be called here or SDL_PumpEvents() directly.

    padStates[player].btn = getButtons(player);

    if (gamepads[player]) {
        padStates[player].stat = 0x00; // Example: Connected, normal status
        padStates[player].len = 0x05;  // Typical length for analog controller data
        padStates[player].type = 0x07; // Example: 0x4 = Digital, 0x5 = Analog Stick, 0x7 = DualShock/Full Analog

        Sint16 lx = SDL_GetGamepadAxis(gamepads[player], SDL_GAMEPAD_AXIS_LEFTX);
        Sint16 ly = SDL_GetGamepadAxis(gamepads[player], SDL_GAMEPAD_AXIS_LEFTY);
        Sint16 rx = SDL_GetGamepadAxis(gamepads[player], SDL_GAMEPAD_AXIS_RIGHTX);
        Sint16 ry = SDL_GetGamepadAxis(gamepads[player], SDL_GAMEPAD_AXIS_RIGHTY);

        // Convert Sint16 (-32768 to 32767) to unsigned char (0 to 255)
        // (value / 256) + 128 maps approx -32768..32767 to 0..255
        padStates[player].ls_x = (unsigned char)((lx / 256) + 128);
        padStates[player].ls_y = (unsigned char)((ly / 256) + 128);
        padStates[player].rs_x = (unsigned char)((rx / 256) + 128);
        padStates[player].rs_y = (unsigned char)((ry / 256) + 128);

    } else {
        // No gamepad for this player, or player is inactive
        // If player == 0, this means keyboard only (or no input if no kb buttons pressed)
        padStates[player].stat = 0x00; // Still okay status
        padStates[player].len = 0x02;  // Typical for digital (buttons only)
        padStates[player].type = 0x04; // Digital type
        
        padStates[player].ls_x = 128; // Centered
        padStates[player].ls_y = 128; // Centered
        padStates[player].rs_x = 128; // Centered
        padStates[player].rs_y = 128; // Centered
    }

    return &padStates[player];
}

// Optional: Cleanup function
void closeInput() {
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        if (gamepads[i]) {
            SDL_CloseGamepad(gamepads[i]);
            gamepads[i] = NULL;
        }
    }
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD);
    SDL_Log("Input systems closed.");
}

```
