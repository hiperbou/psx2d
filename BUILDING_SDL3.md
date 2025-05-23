# Building with SDL3 Backend

This document provides instructions for installing SDL3 and its related libraries to compile the project using the SDL3 backend (`Makefile.sdl3`).

## Overview

The project uses `sdl3-config` to automatically determine compiler and linker flags for SDL3. If `sdl3-config` is not available (e.g., from a manual installation or some package managers), you may need to set `SDL_CFLAGS` and `SDL_LIBS` manually in `Makefile.sdl3` or as environment variables.

You will need:
1.  **SDL3 development library**: Core SDL3 library.
2.  **SDL3_image development library**: For loading various image formats (used by `IMG_Load`).

## Linux Instructions

### Using Package Manager (Recommended for Debian/Ubuntu based systems)

1.  **Update package list:**
    ```bash
    sudo apt-get update
    ```

2.  **Install SDL3 development libraries:**
    As of the creation of this document, official SDL3 packages might not be in stable repositories. You might find them in testing/unstable branches, PPAs, or they might be named `libsdl3-dev` and `libsdl3-image-dev`.

    Example (names might vary):
    ```bash
    sudo apt-get install libsdl3-dev libsdl3-image-dev
    ```
    If these specific packages are not found, you may need to build from source (see below) or find a PPA that provides them for your distribution.

### Building SDL3 and SDL3_image from Source (General Linux)

If packages are not available or you need the latest version:

1.  **Install build dependencies:**
    You'll need common build tools and libraries. For Debian/Ubuntu:
    ```bash
    sudo apt-get install build-essential git cmake ninja-build libasound2-dev libpulse-dev libaudio-dev libjack-dev libsndio-dev libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libwayland-dev libdrm-dev libgbm-dev libdbus-1-dev libudev-dev libgl1-mesa-dev libvulkan-dev libpipewire-0.3-dev
    ```
    (The list might vary; check SDL's documentation for the most up-to-date dependencies.)

2.  **Clone, Build, and Install SDL3:**
    ```bash
    git clone https://github.com/libsdl-org/SDL.git -b SDL3 # Or download a release tarball
    cd SDL
    mkdir build && cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local -DSDL_SHARED=ON -DSDL_STATIC=OFF # Adjust prefix if needed
    # Or using the classic configure script if available in the release:
    # ../configure --prefix=/usr/local
    make -j$(nproc)
    sudo make install
    ```

3.  **Clone, Build, and Install SDL3_image:**
    ```bash
    cd ../.. # Back to where you cloned SDL
    git clone https://github.com/libsdl-org/SDL_image.git -b SDL3 # Or download a release tarball
    cd SDL_image
    mkdir build && cd build
    # Important: Point to your SDL3 installation if not in standard path for cmake
    # For example, if SDL3 was installed to /usr/local:
    cmake .. -DCMAKE_PREFIX_PATH=/usr/local -DCMAKE_INSTALL_PREFIX=/usr/local -DSDL3_DIR=/usr/local/lib/cmake/SDL3
    # Or for the classic configure script:
    # ../configure --prefix=/usr/local --with-sdl-prefix=/usr/local
    make -j$(nproc)
    sudo make install
    ```

4.  **Update library cache:**
    After installing to a custom prefix like `/usr/local`, ensure your system can find the new libraries:
    ```bash
    sudo ldconfig
    ```

## Windows Instructions

### Using MSYS2 with MinGW-w64 (Recommended)

MSYS2 provides a Linux-like environment and package management on Windows.

1.  **Install MSYS2:**
    Follow instructions on [www.msys2.org](https://www.msys2.org/).

2.  **Open MSYS2 MinGW 64-bit (or 32-bit) Shell:**
    Use the appropriate shell for your target architecture (e.g., `ucrt64.exe` for MinGW UCRT 64-bit).

3.  **Update package database and install SDL3:**
    ```bash
    pacman -Syu # Update system first, may require closing and reopening shell
    pacman -S mingw-w64-x86_64-SDL3 mingw-w64-x86_64-SDL3_image # For 64-bit
    # For 32-bit: pacman -S mingw-w64-i686-SDL3 mingw-w64-i686-SDL3_image
    ```
    This should install SDL3 and SDL3_image along with their dependencies, and `sdl3-config` should be available in the MinGW environment.

### Manual Installation for MinGW (Without MSYS2 package manager)

1.  **Download SDL3 Development Libraries:**
    Go to the [SDL releases page](https://github.com/libsdl-org/SDL/releases) and download the MinGW development libraries (e.g., `SDL3-devel-X.Y.Z-mingw.tar.gz`).
    Do the same for [SDL_image releases](https://github.com/libsdl-org/SDL_image/releases) (e.g., `SDL3_image-devel-X.Y.Z-mingw.tar.gz`).

2.  **Extract Files:**
    Extract the archives. You'll find `include` and `lib` directories.

3.  **Place Files in MinGW Installation:**
    Copy the contents of the `include/SDL3` directory from the extracted SDL3 archive to your MinGW compiler's `include/SDL3` directory (e.g., `C:\MinGW\include\SDL3`).
    Copy the contents of the `include/SDL3` directory from SDL3_image (it should also be `SDL_image.h` directly in `include`) to `C:\MinGW\include\SDL3` (or just `C:\MinGW\include`).
    Copy the library files (`.a`, `.dll`) from `lib/x64` (or `lib/x86`) to your MinGW compiler's `lib` directory (e.g., `C:\MinGW\lib`). The `.dll` files also need to be in your `PATH` or in the same directory as your executable when running it.

4.  **Manual Makefile Configuration:**
    If `sdl3-config` is not available, you'll need to manually edit `Makefile.sdl3`:
    ```makefile
    # Example for MinGW if headers are in C:/MinGW/include and libs in C:/MinGW/lib
    SDL_CFLAGS = -IC:/MinGW/include/SDL3
    SDL_LIBS = -LC:/MinGW/lib -lSDL3 -lSDL3main -lSDL3_image # Order might matter
    ```
    Note: `SDL3main` is often needed for MinGW.

## macOS Instructions

### Using Homebrew (Recommended)

1.  **Install Homebrew:**
    Follow instructions on [brew.sh](https://brew.sh/).

2.  **Install SDL3:**
    SDL3 might be available via `brew install sdl3 sdl3_image`. Check Homebrew formulas for the exact names.

### Building SDL3 from Source (macOS)

Similar to Linux:
1.  Install Xcode Command Line Tools: `xcode-select --install`.
2.  Follow the "Building SDL3 and SDL3_image from Source (General Linux)" steps, using `cmake` or `./configure`. SDL3 uses frameworks on macOS, so paths might differ slightly, but `sdl3-config` or `cmake` should handle it.
    The default prefix `/usr/local` usually works well with Homebrew and macOS.

## Verifying Installation

After installation, the `Makefile.sdl3` should ideally find `sdl3-config`. You can test this:
```bash
sdl3-config --cflags
sdl3-config --libs
```
If these commands output the correct flags, the Makefile should work without modification. If not, you might need to adjust your `PATH` environment variable to include the directory where `sdl3-config` was installed (e.g., `/usr/local/bin`) or manually set the flags in the Makefile.

## Compiling the Project

Once dependencies are installed:
```bash
make -f Makefile.sdl3
```
This will create the `game_sdl3` executable in the project root.
```
