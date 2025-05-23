#include <SDL3/SDL.h>

#include "../fpg.h"         // For FPG_ST, FPGHeader, etc. (general FPG definitions)
#include "./sdl3_fpg.h"     // For the SDL3-specific SPRITE struct definition
// #include "../../core/hgl_types.h" // Likely not needed for CopySprite directly
// #include "../../engine/sprites.h"   // If it defines a generic SPRITE, ensure no conflict. Here we use sdl3_fpg.h's
// #include "../../pool/FixedPool.h" // Not directly used in CopySprite, but might be for FPG loading/management

/**
 * @brief Copies the contents of one SPRITE structure to another.
 *
 * This function performs a shallow copy. The SDL_Texture pointer (m_image)
 * is copied, not duplicated. This means both SPRITE structures will
 * point to the same texture data. This is usually the desired behavior
 * as textures are managed resources.
 *
 * @param original Pointer to the source SPRITE structure.
 * @param sprite Pointer to the destination SPRITE structure.
 */
void CopySprite(SPRITE *original, SPRITE *sprite) {
    if (original == NULL || sprite == NULL) {
        // SDL_Log or your engine's equivalent error logging can be added here if desired
        // For example: SDL_Log("CopySprite: original or sprite pointer is NULL.");
        return;
    }

    // Copy the texture pointer
    sprite->m_image = original->m_image;

    // Copy texture coordinates and dimensions
    sprite->u = original->u;
    sprite->v = original->v;
    sprite->w = original->w;
    sprite->h = original->h;

    // If other optional members were added to the SPRITE struct in sdl3_fpg.h,
    // they should be copied here as well. For example:
    // sprite->colorKey = original->colorKey;
    // sprite->scale_x = original->scale_x;
    // sprite->scale_y = original->scale_y;
    // sprite->angle = original->angle;
    // sprite->center = original->center;
}

// Other FPG related functions specific to SDL3 that might be needed later:
// - Functions to load FPG files and populate FPG_ST structures with SDL_Textures
//   (these would be analogues to what's in fpg.c but using SDL for texture creation)
// - Functions to free FPG_ST structures, including destroying SDL_Textures

// Example of what a function in fpg.c might call, adapted for SDL3:
// (This is illustrative and would be part of a larger FPG loading system)
/*
SPRITE* load_fpg_sprite_sdl3(FPG_ST* fpg_file, int map_index) {
    if (!fpg_file || !fpg_file->map[map_index] || !fpg_file->map[map_index]->image_data) {
        return NULL;
    }
    
    // This is where image_data (raw pixels) would be converted to an SDL_Texture
    // For example, using GetSpriteFromMemory or similar logic from sdl3_hgl_file.c
    // This is simplified; actual FPG loading is more complex.

    SPRITE* new_sprite = (SPRITE*)SDL_malloc(sizeof(SPRITE));
    if (!new_sprite) return NULL;

    // Assuming fpg_file->map[map_index]->image_data is somehow processed into a texture
    // and its properties are set (u,v,w,h).
    // For now, this is a placeholder. The actual loading logic (GetSpriteFromDisc,
    // GetSpriteFromMemory) is in sdl3_hgl_file.c.
    // This CopySprite is more for duplicating an *already loaded* SPRITE instance.

    // Let's assume a hypothetical function that creates/gets an SDL_Texture for an FPG entry
    // SDL_Texture* tex = create_texture_for_fpg_entry(fpg_file->map[map_index]);
    // new_sprite->m_image = tex;
    // new_sprite->u = fpg_file->map[map_index]->u; // Or however these are stored
    // new_sprite->v = fpg_file->map[map_index]->v;
    // new_sprite->w = fpg_file->map[map_index]->width;
    // new_sprite->h = fpg_file->map[map_index]->height;
    
    return new_sprite;
}
*/
