# Building the Project with SDL3 Backend

## Introduction

This document provides instructions on how to set up your development environment to compile this project using the SDL3 backend. SDL3 (Simple DirectMedia Layer) is a cross-platform development library designed to provide low-level access to audio, keyboard, mouse, joystick, and graphics hardware via OpenGL and Direct3D.

## General Requirements

Before you begin, ensure you have the following installed:

*   **C99 Compatible C Compiler:** Such as GCC (GNU Compiler Collection) or Clang.
*   **`make` Build Utility:** For processing the Makefile.
*   **`sdl3-config` Utility (Recommended):** This script helps the Makefile find the necessary compiler flags and linker libraries for SDL3 and its satellite libraries like SDL3_image. If `sdl3-config` is not available or not used, you will need to manually configure include paths and library paths in `Makefile.sdl3`.

## Linux Setup

The following instructions are for common Linux distributions. Package names for SDL3 and SDL3_image might vary or might not be available in official repositories until SDL3 is officially released and packaged. Always check your distribution's package manager for the latest official names.

*   **Ubuntu/Debian:**
    ```bash
    sudo apt update
    sudo apt install build-essential libsdl3-dev libsdl3-image-dev
    # Adjust package names as they become official (e.g., libsdl3-0, libsdl3-image-0 for runtime if separate)
    # The -dev packages should ideally include sdl3-config.
    ```

*   **Fedora:**
    ```bash
    sudo dnf install gcc make SDL3-devel SDL3_image-devel
    # Adjust package names as per official Fedora repositories.
    ```

*   **Arch Linux:**
    ```bash
    sudo pacman -Syu gcc make sdl3 sdl3_image
    # Adjust package names as per official Arch repositories/AUR.
    ```

*   **Note on `sdl3-config`:**
    The `Makefile.sdl3` uses `sdl3-config --cflags` and `sdl3-config --libs` to automatically determine the correct compiler and linker flags. For this to work, the `sdl3-config` script (and `sdl3-config` for SDL3_image, which might be part of the main SDL3 script or a separate one) must be installed and accessible in your system's `PATH`.
    If `sdl3-config` is installed to a non-standard location (e.g., `/usr/local/bin` and this isn't in your `PATH`), you might need to add it to your `PATH` or modify `Makefile.sdl3` to point directly to the script or manually set the `CFLAGS` and `LDLIBS`.

## Windows Setup (MinGW/GCC)

These instructions are for setting up a MinGW-w64 based GCC environment on Windows.

*   **Compiler (MinGW-w64):**
    1.  **Install MSYS2:** Go to [www.msys2.org](https://www.msys2.org/) and download the MSYS2 installer.
    2.  Follow the installation instructions.
    3.  Once installed, open an MSYS2 MINGW64 shell (not the MSYS shell).
    4.  Update the package database and core packages:
        ```bash
        pacman -Syu
        # You might need to close and reopen the shell if core MSYS2 packages are updated.
        pacman -Su
        ```
    5.  Install the MinGW-w64 GCC toolchain and `make`:
        ```bash
        pacman -S mingw-w64-x86_64-toolchain make
        # This installs gcc, make, and other development tools.
        ```
    6.  Ensure that the MinGW-w64 `bin` directory (e.g., `C:\msys64\mingw64\bin`) is added to your Windows system `PATH` environment variable if you plan to build from `cmd.exe` or PowerShell. If using the MSYS2 MINGW64 shell, this is typically set up automatically.

*   **SDL3 Development Libraries:**
    1.  Go to the official SDL GitHub releases page: [https://github.com/libsdl-org/SDL/releases](https://github.com/libsdl-org/SDL/releases)
    2.  Download the latest SDL3 development library archive for MinGW (e.g., `SDL3-devel-X.Y.Z-mingw.tar.gz` or `.zip`).
    3.  Go to the SDL_image GitHub releases page: [https://github.com/libsdl-org/SDL_image/releases](https://github.com/libsdl-org/SDL_image/releases)
    4.  Download the latest SDL3_image development library archive for MinGW (e.g., `SDL3_image-devel-X.Y.Z-mingw.tar.gz` or `.zip`).

*   **Installation of SDL3 Libraries:**
    1.  Create a directory to store these libraries. For example, create `third_party/sdl3_mingw` in your project's root directory.
        ```bash
        mkdir -p third_party/sdl3_mingw
        ```
    2.  Extract the contents of the downloaded SDL3 and SDL3_image development archives into this `third_party/sdl3_mingw` directory.
    3.  After extraction, your directory structure should look something like this (the exact structure depends on how the development archives are packaged):
        ```
        third_party/
            sdl3_mingw/
                include/
                    SDL3/
                        SDL.h
                        SDL_main.h
                        ... (other SDL3 headers)
                    SDL3_image/  # Or headers might be directly in include/SDL3/
                        SDL_image.h
                        ...
                lib/
                    libSDL3.a
                    libSDL3main.a
                    libSDL3_image.a
                    SDL3.dll      # Runtime DLL
                    SDL3_image.dll # Runtime DLL
                    ...
                bin/
                    sdl3-config   # The configuration script
                    SDL3.dll      # Often also here
                    SDL3_image.dll # Often also here
                share/            # Might contain documentation or other files
                    ...
        ```
        **Note:** Pay close attention to the structure within the archives. You might need to adjust the paths in the Makefile or consolidate files if SDL3_image headers are in a different location (e.g., directly under `include/SDL3/`). The `Makefile.sdl3` assumes `sdl3-config` will provide correct paths.

    4.  **Using `sdl3-config` (Recommended):**
        *   The `sdl3-config` script is usually found in the `bin/` directory of the extracted SDL3 development library.
        *   For `make` to find `sdl3-config`, you must add the path to this `bin` directory (e.g., `your_project_root/third_party/sdl3_mingw/bin`) to your system `PATH` when building.
        *   Alternatively, you can modify `Makefile.sdl3` to explicitly call `sdl3-config` with its full path.

    5.  **Manual Makefile Configuration (If `sdl3-config` is not used/found):**
        *   If you cannot use `sdl3-config`, you'll need to manually edit `Makefile.sdl3`.
        *   Uncomment and adjust the placeholder lines for `CFLAGS` and `LDLIBS`:
            ```makefile
            # Example manual overrides in Makefile.sdl3
            # Remove or comment out the sdl3-config shell calls for CFLAGS and LDLIBS
            # Then add:
            # CFLAGS += -Ithird_party/sdl3_mingw/include
            # LDLIBS += -Lthird_party/sdl3_mingw/lib -lSDL3main -lSDL3 -lSDL3_image
            # Note: Order of libraries can matter. SDL3main is often needed.
            ```
        *   You'll also need to ensure any necessary preprocessor defines provided by `sdl3-config` are added to `CFLAGS`.

    6.  **Runtime DLLs:**
        *   To run the compiled game (`sdl3_game.exe`), the runtime libraries (`SDL3.dll`, `SDL3_image.dll`, and any dependencies they have like `libpng.dll`, `libjpeg.dll` etc. from SDL_image) must be accessible.
        *   Copy these `.dll` files from `third_party/sdl3_mingw/bin` (or `lib` where they might also be) to the same directory as your compiled `sdl3_game.exe`.
        *   Alternatively, ensure the directory containing these DLLs is in your system `PATH`.

## Building the Project (SDL3 Backend)

Once the compiler, make, and SDL3 dependencies are set up:

1.  Open a terminal or command prompt (e.g., MSYS2 MINGW64 shell on Windows).
2.  Navigate to the root directory of this project.
3.  Run the make command, specifying `Makefile.sdl3`:
    ```bash
    make -f Makefile.sdl3
    ```
4.  If the build is successful, the executable will be created in the project's root directory:
    *   On Linux/macOS: `sdl3_game`
    *   On Windows: `sdl3_game.exe`

## Troubleshooting

*   **`make: sdl3-config: Command not found` (or similar for `sh`):**
    *   **Solution:** `sdl3-config` is not in your system `PATH` or not installed.
        *   Ensure you've installed the SDL3 development package (`libsdl3-dev` or `SDL3-devel`).
        *   If installed manually (e.g., on Windows), ensure the `bin` directory containing `sdl3-config` is in your `PATH`.
        *   Alternatively, edit `Makefile.sdl3` to provide the full path to `sdl3-config` or set `CFLAGS` and `LDLIBS` manually (see Windows setup).

*   **`fatal error: SDL3/SDL.h: No such file or directory` (or `SDL_image.h`):**
    *   **Solution:** The compiler cannot find the SDL3 (or SDL3_image) header files.
        *   Verify that `sdl3-config --cflags` (run this command manually in your terminal) outputs the correct `-I<path_to_sdl3_include_dir>`.
        *   If you configured `Makefile.sdl3` manually, double-check the `-I` paths in `CFLAGS`.
        *   Ensure the SDL3/SDL3_image development libraries are correctly installed/extracted.

*   **Linker errors (`undefined reference to SDL_...` or `SDL_image_...`):**
    *   **Solution:** The linker cannot find the SDL3 (or SDL3_image) libraries, or they are not being linked correctly.
        *   Verify that `sdl3-config --libs` (run this command manually) outputs the correct `-L<path_to_sdl3_lib_dir> -lSDL3 -lSDL3_image` (and other necessary libs like `-lSDL3main`).
        *   If you configured `Makefile.sdl3` manually, check the `-L` and `-l` flags in `LDLIBS`. Ensure the order of libraries is correct (e.g., `-lSDL3main -lSDL3`).
        *   On Windows, ensure you are linking against the `.a` import libraries and that the corresponding `.dll` files are available.

*   **Runtime errors (`SDL3.dll not found` or similar):**
    *   **Solution (Windows):** Ensure `SDL3.dll`, `SDL3_image.dll`, and their dependencies are in the same directory as `sdl3_game.exe` or in a directory included in your system `PATH`.
