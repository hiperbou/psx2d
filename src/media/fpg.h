#ifndef __HGL_FPG__
#define __HGL_FPG__

#ifdef PSX
#include "psx/psx_fpg.h"
#else
#include "ctoy/ctoy_fpg.h"
#endif

#include <stdint.h>
//#include "system.h"
//#include <sys/types.h>	// This provides typedefs needed by libgte.h and libgpu.h
#include "../pool/FixedPool.h"

#define MAX_NUM_FPGS 8
#define MAX_NUM_MAPS 1024

typedef struct{
        //SDL_Surface *image;
        SPRITE *image;
        //Uint16 w;
        //Uint16 h;
        //GLuint imagegl;
        //Uint16 posx; //pos inicial para texturas de opengl
        //Uint16 posy;
        //Uint16 flag;
        //tiled_map mapas;
        //Uint16 th; //numero de tiles horizontales
        //Uint16 tv; //numero de tiles verticales
        //Uint16 tsize;
        //GLuint *tile_text; //array de texturas
        //unsigned short *center_x;
        //unsigned short *center_y;
        //int num_points;
}div_map;

typedef struct{
       div_map *map [MAX_NUM_MAPS]; 
       int mapa_libre;
       int total_mapas;
       //Uint16 texture_size;   
}div_fpg;

extern div_fpg *fpg[MAX_NUM_FPGS];

extern FixedPool *mapPool;

int init_fpgs();
int new_fpg();
void unload_fpg( int pos );

int load_map(int file, char *filename);
int load_map_from_memory(int file, uint8_t *data);
//inline int load_map(u_char *data){return load_map(0,name);}  

int load_atlas(int file, char *filename, int tileWidth, int tileHeight, int numCols, int numRows);

void unload_map(int,int);
int get_free_map(int file); //devuelve el proximo mapa libre del fpg

#endif