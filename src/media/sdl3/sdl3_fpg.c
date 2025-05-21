#include <SDL3/SDL_render.h> // For SDL_Texture, SDL_DestroyTexture
#include <stdio.h>    // For printf (debugging)
#include <stdlib.h>   // For malloc, free (for unique texture list)

#include "../fpg.h"   // Includes SPRITE definition from sdl3/sdl3_fpg.h and extern div_fpg *fpg[]
// #include "../../core/hgl_file.h" // This is where GetSpriteFromDisc etc are declared.
                                 // We need FreeSprite. Let's assume it's implicitly available or handle manually.

// Forward declaration for FreeSprite if its header isn't included.
// FreeSprite is in src/core/sdl3/sdl3_hgl_file.c
// A proper header for sdl3_hgl_file.c content would be better.
#ifndef FREE_SPRITE_DECLARED
#define FREE_SPRITE_DECLARED
// SPRITE is defined via fpg.h -> sdl3/sdl3_fpg.h
void FreeSprite(SPRITE *sprite) {
    if (sprite && sprite->texture) {
        SDL_DestroyTexture(sprite->texture);
        sprite->texture = NULL;
        sprite->w = 0;
        sprite->h = 0;
    }
}
#endif


void CopySprite(SPRITE *original, SPRITE *destination) {
    if (!original || !destination) {
        return;
    }
    // This is a shallow copy for the texture pointer.
    // The texture itself is not duplicated.
    destination->texture = original->texture;
    destination->u = original->u;
    destination->v = original->v;
    destination->w = original->w;
    destination->h = original->h;
}

void unload_map(int fpg_idx, int map_idx) {
    if (fpg_idx < 0 || fpg_idx >= MAX_NUM_FPGS || fpg[fpg_idx] == NULL) {
        // printf("unload_map: Invalid fpg_idx %d\n", fpg_idx);
        return;
    }
    if (map_idx < 0 || map_idx >= MAX_NUM_MAPS || fpg[fpg_idx]->map[map_idx] == NULL) {
        // printf("unload_map: Invalid map_idx %d for fpg %d\n", map_idx, fpg_idx);
        return;
    }

    SPRITE *sprite_to_free = fpg[fpg_idx]->map[map_idx]->image;
    if (sprite_to_free) {
        // Calling FreeSprite will destroy the texture.
        // If this texture is shared (e.g., an atlas), this will invalidate other sprites
        // using the same texture. This is a design consideration for the game.
        // The FreeSprite function sets sprite_to_free->texture to NULL.
        FreeSprite(sprite_to_free); 
    }

    // The div_map and SPRITE structures themselves were allocated from FixedPools
    // in the common fpg.c. This backend-specific function cannot free them
    // back to those pools without an interface from fpg.c.
    // So, we only nullify the entry in the fpg's map array.
    // The memory for fpg[fpg_idx]->map[map_idx]->image (SPRITE struct) and 
    // fpg[fpg_idx]->map[map_idx] (div_map struct) is effectively leaked from the pool's perspective
    // until the pool is reset, or if fpg.c's allocation logic can reuse NULL map slots.
    
    // fpg[fpg_idx]->map[map_idx]->image = NULL; // FreeSprite already handles the sprite content
    // free(fpg[fpg_idx]->map[map_idx]->image); // Don't free pool-allocated memory with free()
    // free(fpg[fpg_idx]->map[map_idx]);       // Don't free pool-allocated memory with free()
    fpg[fpg_idx]->map[map_idx] = NULL; // Mark the map slot as empty.

    // Adjust total_mapas, mapa_libre in fpg[fpg_idx] would be complex here
    // and is part of fpg.c's responsibility. This function just handles resources.
    // fpg[fpg_idx]->total_mapas--; // Incorrect without careful management of map array
}

void unload_fpg(int fpg_idx) {
    if (fpg_idx < 0 || fpg_idx >= MAX_NUM_FPGS || fpg[fpg_idx] == NULL) {
        // printf("unload_fpg: Invalid fpg_idx %d\n", fpg_idx);
        return;
    }

    // Step 1: Collect unique textures
    SDL_Texture* unique_textures[MAX_NUM_MAPS]; // Max possible different textures in one FPG
    int unique_texture_count = 0;

    for (int i = 0; i < MAX_NUM_MAPS; ++i) { // Iterate up to MAX_NUM_MAPS as total_mapas might not be reliable
        if (fpg[fpg_idx]->map[i] != NULL &&
            fpg[fpg_idx]->map[i]->image != NULL &&
            fpg[fpg_idx]->map[i]->image->texture != NULL) {
            
            SDL_Texture* current_tex = fpg[fpg_idx]->map[i]->image->texture;
            int found = 0;
            for (int j = 0; j < unique_texture_count; ++j) {
                if (unique_textures[j] == current_tex) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (unique_texture_count < MAX_NUM_MAPS) { // Boundary check
                    unique_textures[unique_texture_count++] = current_tex;
                } else {
                    // This case should ideally not happen if MAX_NUM_MAPS is large enough
                    // or indicates an issue. For now, we'd just not store more.
                    // printf("unload_fpg: Exceeded unique_textures array capacity for fpg %d\n", fpg_idx);
                }
            }
        }
    }

    // Step 2: Destroy unique textures
    for (int i = 0; i < unique_texture_count; ++i) {
        SDL_DestroyTexture(unique_textures[i]);
    }

    // Step 3: Nullify texture pointers in all sprites and free/nullify map entries
    for (int i = 0; i < MAX_NUM_MAPS; ++i) {
        if (fpg[fpg_idx]->map[i] != NULL) {
            if (fpg[fpg_idx]->map[i]->image != NULL) {
                fpg[fpg_idx]->map[i]->image->texture = NULL; // Texture is destroyed, nullify pointer
                fpg[fpg_idx]->map[i]->image->w = 0;
                fpg[fpg_idx]->map[i]->image->h = 0;
                // The SPRITE struct itself (fpg[fpg_idx]->map[i]->image) is pool-allocated.
                // Cannot free it here with standard free().
            }
            // The div_map struct itself (fpg[fpg_idx]->map[i]) is pool-allocated.
            // Cannot free it here.
            fpg[fpg_idx]->map[i] = NULL; // Mark map slot as empty
        }
    }
    
    // The div_fpg struct (fpg[fpg_idx]) itself is pool-allocated in fpg.c.
    // Cannot free it here.
    // fpg[fpg_idx] = NULL; // This would be done by whoever manages the fpg array at a higher level.
    // The task implies this function should free fpg[fpg_idx] and set it to NULL.
    // However, given it's from a FixedPool in fpg.c, this backend code cannot.
    // The common fpg.c should provide an "fpg_delete" or similar that handles the pool.
    // For now, this function will just clear its contents as much as possible from the backend side.
    
    fpg[fpg_idx]->mapa_libre = 0; // Reset for potential reuse if fpg slot is reused by fpg.c
    fpg[fpg_idx]->total_mapas = 0;
}

// Note: The functions init_fpgs, new_fpg, load_map, load_map_from_memory, and load_atlas
// are implemented in the common src/media/fpg.c and should not be re-implemented here.
// This file provides the backend-specific implementations for functions not covered
// by fpg.c or those that fpg.c delegates (like CopySprite indirectly).

```
