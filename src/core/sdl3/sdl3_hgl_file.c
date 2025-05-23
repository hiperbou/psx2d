#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h> // For IMG_Load

#include "../hgl_file.h"     // Likely contains declarations for these functions
#include "../../media/fpg.h" // Contains SPRITE definition (or a compatible one for now)
#include "../../utils/utils.h" // For utility functions if needed (e.g., path joining)

// External SDL_Renderer needed for texture creation. 
// This should be made available, e.g., via a global or passed in.
// For now, assume it's accessible. A better approach would be to pass it to functions
// that need it, or have an accessor function.
extern SDL_Renderer *renderer; // This is defined in sdl3_hgl.c

// Helper to set sprite properties from a texture
void GetSprite(SDL_Texture *texture, SPRITE *sprite) {
    if (!texture || !sprite) {
        SDL_Log("GetSprite: Invalid texture or sprite pointer.\n");
        return;
    }

    sprite->m_image = texture; // Store the SDL_Texture

    // Get texture dimensions
    int w, h;
    if (SDL_QueryTexture(texture, NULL, NULL, &w, &h) != 0) {
        SDL_Log("GetSprite: Failed to query texture attributes: %s\n", SDL_GetError());
        // Set to zero or some default error state
        sprite->w = 0;
        sprite->h = 0;
    } else {
        sprite->w = w;
        sprite->h = h;
    }

    // UV coordinates for the whole texture
    sprite->u = 0;
    sprite->v = 0;
    // sprite->u2 = w; // If SPRITE stores texture rectangle coordinates
    // sprite->v2 = h; 
}

// Load sprite from a file
int GetSpriteFromDisc(const char *filename, SPRITE *sprite) {
    if (!filename || !sprite) {
        SDL_Log("GetSpriteFromDisc: Invalid filename or sprite pointer.\n");
        return -1; 
    }

    if (!renderer) {
        SDL_Log("GetSpriteFromDisc: Renderer not initialized.\n");
        return -1;
    }

    // IMG_Load can load various image formats (PNG, JPG, etc.)
    SDL_Surface *surface = IMG_Load(filename);
    if (!surface) {
        SDL_Log("GetSpriteFromDisc: Unable to load image %s! SDL_image Error: %s\n", filename, IMG_GetError());
        return -1;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("GetSpriteFromDisc: Unable to create texture from %s! SDL Error: %s\n", filename, SDL_GetError());
        SDL_DestroySurface(surface);
        return -1;
    }

    // Populate the sprite structure using the new texture
    GetSprite(texture, sprite);

    // Free the loaded surface as it's no longer needed
    SDL_DestroySurface(surface);

    // Check if GetSprite succeeded in setting dimensions
    if (sprite->w == 0 && sprite->h == 0 && texture != NULL) {
        // This indicates an issue in GetSprite's SDL_QueryTexture or logic
        SDL_Log("GetSpriteFromDisc: Texture loaded but sprite dimensions are zero.\n");
        // Potentially destroy texture if this is considered a fatal error for this function
        // SDL_DestroyTexture(texture);
        // sprite->m_image = NULL;
        // return -1;
    }
    
    return 0; // Success
}

// Load sprite from memory data
// This is a more complex function. For now, a basic structure or placeholder.
int GetSpriteFromMemory(uint8_t *data, int size, SPRITE *sprite) {
    if (!data || size <= 0 || !sprite) {
        SDL_Log("GetSpriteFromMemory: Invalid data, size, or sprite pointer.\n");
        return -1;
    }

    if (!renderer) {
        SDL_Log("GetSpriteFromMemory: Renderer not initialized.\n");
        return -1;
    }

    // Create an SDL_RWops structure from the memory data
    SDL_RWops *rw = SDL_CreateRWFromConstMem(data, size);
    if (!rw) {
        SDL_Log("GetSpriteFromMemory: Unable to create RWops from memory: %s\n", SDL_GetError());
        return -1;
    }

    // Load the image using IMG_Load_RW
    // The second argument '1' means SDL_RWclose will be called on rw when the surface is freed.
    SDL_Surface *surface = IMG_Load_RW(rw, 1); 
    if (!surface) {
        SDL_Log("GetSpriteFromMemory: Unable to load image from memory! SDL_image Error: %s\n", IMG_GetError());
        // Note: rw is automatically closed by IMG_Load_RW on failure if 'autoclose' is true.
        // If IMG_Load_RW itself fails before even trying to use rw, it might not be closed.
        // However, modern SDL_image usually handles this well.
        // If not using autoclose (0), then SDL_DestroyRW(rw) would be needed here.
        return -1;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("GetSpriteFromMemory: Unable to create texture from memory surface! SDL Error: %s\n", SDL_GetError());
        SDL_DestroySurface(surface); // Surface is not freed automatically if texture creation fails
        return -1;
    }

    GetSprite(texture, sprite);
    SDL_DestroySurface(surface);

    if (sprite->w == 0 && sprite->h == 0 && texture != NULL) {
         SDL_Log("GetSpriteFromMemory: Texture loaded from memory but sprite dimensions are zero.\n");
    }

    return 0; // Success
}

// Placeholder for HGL_FileOpen - actual implementation depends on how files are managed/packed
void* HGL_FileOpen(const char* filename, const char* mode) {
    // For SDL, this might map to SDL_RWFromFile if dealing with raw file access,
    // or it might be part of a larger virtual file system.
    // For now, let's assume it uses SDL_RWops.
    // The 'mode' could be "rb", "wb", etc.
    return (void*)SDL_RWFromFile(filename, mode);
}

void HGL_FileClose(void* stream) {
    if (stream) {
        SDL_RWclose((SDL_RWops*)stream);
    }
}

size_t HGL_FileRead(void *ptr, size_t size, size_t nmemb, void *stream) {
    if (!stream) return 0;
    return SDL_ReadRW((SDL_RWops*)stream, ptr, size, nmemb);
}

int HGL_FileSeek(void *stream, long int offset, int whence) {
    if (!stream) return -1;
    return (int)SDL_SeekRW((SDL_RWops*)stream, offset, whence);
}

long int HGL_FileTell(void *stream) {
    if (!stream) return -1;
    return SDL_TellRW((SDL_RWops*)stream);
}

// Functions required by fpg.c, which are now part of HGL_File interface
unsigned int filelength(int fn) {
    // This function is problematic as it takes an int 'fn' (file number)
    // which is not how SDL_RWops works. It implies a system where files are
    // opened and then referred to by an integer handle.
    // This needs to be adapted to use SDL_RWops* (void* stream) or similar.
    // For now, this is a STUB and indicates a mismatch.
    // A proper implementation would require `fpg.c` to use `HGL_File*` (or `SDL_RWops*`)
    // and functions like `HGL_FileTell` and `HGL_FileSeek` to get file length.
    SDL_Log("filelength(int fn) is deprecated/incompatible with SDL_RWops stream model.\n");
    return 0; 
}

void* load_file(const char* filename, int* filesize) {
    SDL_RWops* rw = SDL_RWFromFile(filename, "rb");
    if (rw == NULL) {
        SDL_Log("load_file: Failed to open %s: %s\n", filename, SDL_GetError());
        if (filesize) *filesize = 0;
        return NULL;
    }

    Sint64 size = SDL_RWsize(rw);
    if (size < 0) {
        SDL_Log("load_file: Failed to get size of %s: %s\n", filename, SDL_GetError());
        SDL_RWclose(rw);
        if (filesize) *filesize = 0;
        return NULL;
    }
    if (size == 0) {
        SDL_Log("load_file: File %s is empty.\n", filename);
        // Still return empty buffer as some code might expect it
    }


    unsigned char* buffer = (unsigned char*)SDL_malloc(size);
    if (buffer == NULL) {
        SDL_Log("load_file: Failed to allocate %lld bytes for %s: %s\n", (long long)size, filename, SDL_GetError());
        SDL_RWclose(rw);
        if (filesize) *filesize = 0;
        return NULL;
    }

    size_t read = SDL_ReadRW(rw, buffer, 1, size);
    if (read != (size_t)size) {
        SDL_Log("load_file: Failed to read %lld bytes from %s (read %zu): %s\n", (long long)size, filename, read, SDL_GetError());
        SDL_free(buffer);
        SDL_RWclose(rw);
        if (filesize) *filesize = 0;
        return NULL;
    }

    SDL_RWclose(rw);
    if (filesize) *filesize = (int)size; // Note: Potential truncation if size > INT_MAX
    return buffer;
}

void unload_file(void *mem) {
    if (mem) {
        SDL_free(mem);
    }
}
