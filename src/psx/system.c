#include <sys/types.h>	// This provides typedefs needed by libgte.h and libgpu.h
#include <stdio.h>	
#include <libetc.h>	// Includes some functions that controls the display
#include <libgte.h>	// GTE header, not really used but libgpu.h depends on it
#include <libgpu.h>	// GPU library header
#include <libcd.h>
#include <stdbool.h>
#include <stddef.h>

#include <strings.h>
#include <ctype.h>

#include "system.h"

// Define environment pairs and buffer counter
DISPENV disp[2];
DRAWENV draw[2];
int db = 0;

#define OTLEN 8         // Ordering table length (recommended to set as a define
                        // so it can be changed easily)
u_long ot[2][OTLEN];    // Ordering table length

char pribuff[2][65536]; // Primitive buffer (32768)
char *nextpri;          // Next primitive pointer

bool isPal() {
    return (*(char *)0xbfc7ff52=='E');  // SCEE string address
}

void initNTSCDisplay() {
        // Configures the pair of DISPENVs for 320x240 mode (NTSC)
    SetDefDispEnv(&disp[0], 0, 0, 320, 240);
    SetDefDispEnv(&disp[1], 0, 240, 320, 240);

    // Configures the pair of DRAWENVs for the DISPENVs
    SetDefDrawEnv(&draw[0], 0, 240, 320, 240);
    SetDefDrawEnv(&draw[1], 0, 0, 320, 240);
}


void initPALDisplay() {
    // Configures the pair of DISPENVs for 320x256 mode (PAL)
    SetDefDispEnv(&disp[0], 0, 0, 320, 256);
    SetDefDispEnv(&disp[1], 0, 256, 320, 256);

    // Screen offset to center the picture vertically
    disp[0].screen.y = 24;
    disp[1].screen.y = disp[0].screen.y;

    // Forces PAL video standard
    SetVideoMode(MODE_PAL);

    // Configures the pair of DRAWENVs for the DISPENVs
    SetDefDrawEnv(&draw[0], 0, 256, 320, 256);
    SetDefDrawEnv(&draw[1], 0, 0, 320, 256);
}

void initClearColor() {
    // Specifies the clear color of the DRAWENV
    setRGB0(&draw[0], 63, 0, 127);
    setRGB0(&draw[1], 63, 0, 127);
    // Enable background clear
    draw[0].isbg = 1;
    draw[1].isbg = 1;
}

void applyEnvironments() {
    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);
}

void clearOrderingTable() {
    ClearOTagR(ot[db], OTLEN);  // Clear ordering table
}

void initDisplay() {
    // Reset GPU and enable interrupts
    ResetGraph(0);

    if (isPal()) {
        printf("Detected PAL System\n");
        initPALDisplay();
    } else {
        printf("Detected NTSC System\n");
        initNTSCDisplay();
    }

    initClearColor();
    // Make sure db starts with zero
    db = 0;
    applyEnvironments();

    nextpri = pribuff[db]; // Set initial primitive pointer address

    clearOrderingTable();
}

void display() {
    // Wait for GPU to finish drawing and V-Blank
    DrawSync(0);
    VSync(0);

    // Apply environments
    applyEnvironments();
    // Enable display
    SetDispMask(1);

    DrawOTag(ot[db] + OTLEN - 1);   // Draw the ordering table
    // Flip buffer counter
    db = !db;
    nextpri = pribuff[db];      // Reset next primitive pointer

    clearOrderingTable();
}

void initFont() {
    // Load the internal font texture
    FntLoad(960, 0);
    // Create the text stream
    FntOpen(0, 8, 320, 224, 0, 100);
}

void drawTexts() {
    FntFlush(-1);
}

void addPrimitive(int x, int y, int w, int h, int r, int g, int b) {
    TILE * tile = (TILE*)nextpri;      // Cast next primitive

    setTile(tile);              // Initialize the primitive (very important)
    setXY0(tile, x, y);       // Set primitive (x,y) position
    setWH(tile, w, h);        // Set primitive size
    setRGB0(tile, r, g, b); // Set color yellow
    addPrim(ot[db], tile);      // Add primitive to the ordering table

    nextpri += sizeof(TILE);    // Advance the next primitive pointer
}

void addSpriteTim(int x, int y,  int w, int h, int tim_uoffs, int tim_voffs, RECT tim_crect) {
    // Sort textured sprite
    SPRT *sprt = (SPRT*)nextpri;

    setSprt(sprt);                  // Initialize the primitive (very important)
    setXY0(sprt, x, y);           // Position the sprite at (48,48)
    setWH(sprt, w, h);            // Set size to 64x64 pixels
    setUV0(sprt,                    // Set UV coordinates
        tim_uoffs, 
        tim_voffs);
    setClut(sprt,                   // Set CLUT coordinates to sprite
        tim_crect.x,
        tim_crect.y);
    setRGB0(sprt,                   // Set primitive color
        128, 128, 128);
    addPrim(ot[db], sprt);          // Sort primitive to OT

    nextpri += sizeof(SPRT);        // Advance next primitive address
}

u_short last_tpage = -1;

char* SortSprite(int x, int y, u_long *ot, char *nextpri, SPRITE *sprite, int uploadTpage) {
    SPRT *sprt = (SPRT*)nextpri;                  // initialize the sprite
    setSprt(sprt);

    setXY0(sprt, x, y);                 // Set position
    setWH(sprt, sprite->w, sprite->h);  // Set size
    setUV0(sprt, sprite->u, sprite->v); // Set UV coordinate of sprite
    setRGB0(sprt,                       // Set the color
        sprite->col.r, 
        sprite->col.g, 
        sprite->col.b);
    sprt->clut = sprite->clut;          // Set the CLUT value
    
    addPrim(ot, sprt);                  // Sort the primitive and advance

    //TODO: check if we are already in the same page to optimize
    if (!uploadTpage/*last_tpage == sprite->tpage*/) {
        return nextpri + sizeof(SPRT);
    } else {
        nextpri += sizeof(SPRT);
        last_tpage = sprite->tpage;

        DR_TPAGE *tpage = (DR_TPAGE*)nextpri;             // Sort the texture page value
        setDrawTPage(tpage, 0, 1, sprite->tpage);
        addPrim(ot, tpage);

        return nextpri+sizeof(DR_TPAGE);        // Return new primitive pointer                                       // (set to nextpri)
    }
}

void addSprite(int x, int y, SPRITE *sprite, int uploadTpage) {
    nextpri = SortSprite(x, y, ot[db], nextpri, sprite, uploadTpage);
}

void SortTile16(int x, int y, u_long *ot, SPRITE *sprite) {
    SPRT_16 *sprt = (SPRT_16*)nextpri;
    setSprt16(sprt);

    setXY0(sprt, x, y);
    setUV0(sprt, sprite->u, sprite->v);
    setRGB0(sprt,
        sprite->col.r, 
        sprite->col.g, 
        sprite->col.b);
    sprt->clut = sprite->clut;
    
    addPrim(ot, sprt);

    nextpri += sizeof(SPRT_16);
}

void SortTile8(int x, int y, u_long *ot, SPRITE *sprite) {
    SPRT_8 *sprt = (SPRT_8*)nextpri;
    setSprt8(sprt);

    setXY0(sprt, x, y);
    setUV0(sprt, sprite->u, sprite->v);
    setRGB0(sprt,
        sprite->col.r, 
        sprite->col.g, 
        sprite->col.b);
    sprt->clut = sprite->clut;
    
    addPrim(ot, sprt);

    nextpri += sizeof(SPRT_8);
}

void addTile16(int x, int y, SPRITE *sprite) {
    SortTile16(x, y, ot[db], sprite);
}

void addTile8(int x, int y, SPRITE *sprite) {
    SortTile8(x, y, ot[db], sprite);
}

void sortRotSprite( int x, int y, int angle, int scale, int flags, SPRITE *sprite, int uploadTpage)
{
    int spriteWidth = sprite->w;
    int spriteHeight = sprite->h;

    int centerX = spriteWidth >> 1;
    int centerY = spriteHeight >> 1;

    int pivotX = spriteWidth >> 1; //TODO: this can be parametrized
    int pivotY = spriteHeight >> 1; //TODO: this can be parametrized

    int centerXScaled = (centerX * scale) >> 12;
    int centerYScaled = (centerY * scale) >> 12;

    int pivotXScaled = (pivotX * scale) >> 12;
    int pivotYScaled = (pivotY * scale) >> 12;

    int pivotXOffset = centerXScaled - pivotXScaled;
    int pivotYOffset = centerYScaled - pivotYScaled;

    int left = -centerXScaled + pivotXOffset;
    int right = centerXScaled + pivotXOffset;
    int top = -centerYScaled + pivotYOffset;
    int bottom = centerYScaled + pivotYOffset;

    SVECTOR	s[4];
    SVECTOR	v[4];

    s[0].vx = left;
    s[0].vy = top;

    s[1].vx = right;
    s[1].vy = top;

    s[2].vx = left;
    s[2].vy = bottom;

    s[3].vx = right;
    s[3].vy = bottom;
    
    int cosAngle = ccos( angle );
    int sinAngle = csin( angle );
    
    for(int i=0; i<4; i++)
    {
        v[i].vx = (((s[i].vx * cosAngle)
            -(s[i].vy * sinAngle))>>12) + x;
        v[i].vy = (((s[i].vy * cosAngle)
            +(s[i].vx * sinAngle))>>12) + y;
    }

    POLY_FT4 *quad = (POLY_FT4*)nextpri;
    setPolyFT4( quad );

    quad->tpage = sprite->tpage;
    quad->clut = sprite->clut; 

    setRGB0( quad, 128, 128, 128 );
    setXY4( quad,
        v[0].vx, v[0].vy,
        v[1].vx, v[1].vy,
        v[2].vx, v[2].vy,
        v[3].vx, v[3].vy );

    int mirroredHorizontal = flags & 1;
    int mirroredVertical = !!(flags & 2);

    int uu = sprite->u + spriteWidth * mirroredHorizontal;
    int vv = sprite->v + spriteHeight * mirroredVertical;
    int ww = spriteWidth  * (1 - 2 * mirroredHorizontal);
    int hh = spriteHeight * (1 - 2 * mirroredVertical);
    setUVWH( quad, uu, vv, ww, hh);

    addPrim( ot[db], quad );
    nextpri += sizeof(POLY_FT4);

    //TODO: check if we are already in the same page to optimize
    if (!uploadTpage /*last_tpage == sprite->tpage*/) {

    } else {
        last_tpage = sprite->tpage;

        DR_TPAGE *tpage = (DR_TPAGE*)nextpri;             // Sort the texture page value
        setDrawTPage(tpage, 0, 1, sprite->tpage);
        addPrim(ot, tpage);

        nextpri += sizeof(DR_TPAGE);    // (set to nextpri)
    }
}

void addRotSprite( int x, int y, int angle, int scale, int flags, SPRITE *sprite, int uploadTpage) {
    sortRotSprite(x, y, angle, scale, flags, sprite, uploadTpage);
}

void sortTpage(u_short newTpage) {
    DR_TPAGE *tpage = (DR_TPAGE*)nextpri;             // Sort the texture page value
    setDrawTPage(tpage, 0, 1, newTpage);
    addPrim(ot[db], tpage);

    nextpri += sizeof(DR_TPAGE);    // (set to nextpri)
}

//deprecated
void setTexturePage(int tim_mode, RECT tim_prect, RECT tim_crect) {
    draw[0].tpage = getTPage( tim_mode&0x3, 0, tim_prect.x, tim_prect.y );
    draw[1].tpage = getTPage( tim_mode&0x3, 0, tim_prect.x, tim_prect.y );
}

void addTexturePage(int tim_mode, RECT tim_prect) {
    DR_TPAGE *tpage = (DR_TPAGE*)nextpri;
    setDrawTPage(tpage, 0, 1,       // Set TPage primitive
    getTPage(tim_mode & 0x3, 0, 
        tim_prect.x, tim_prect.y));

    addPrim(ot[db], tpage);         // Sort primitive to OT

    nextpri += sizeof(DR_TPAGE);    // Advance next primitive address
}


void LoadTexture(u_long * data, TIM_IMAGE * timImage) {     // This part is from Lameguy64's tutorial series : lameguy64.net/svn/pstutorials/chapter1/3-textures.html login/pw: annoyingmous
    OpenTIM(data);                                   // Open the tim binary data, feed it the address of the data in memory
    ReadTIM(timImage);                                // This read the header of the TIM data and sets the corresponding members of the TIM_IMAGE structure
    
    LoadImage(timImage->prect, timImage->paddr);        // Transfer the data from memory to VRAM at position prect.x, prect.y
    DrawSync(0);                                    // Wait for the drawing to end
    
    if (timImage->mode & 0x8){ // check 4th bit       // If 4th bit == 1, TIM has a CLUT
        LoadImage(timImage->crect, timImage->caddr);    // Load it to VRAM at position crect.x, crect.y
        DrawSync(0);                                // Wait for drawing to end
    }
}


void LoadTextureFromDisc(char *filename, TIM_IMAGE * timImage) {
    u_int *filebuff;      // Pointer for the file loaded from the disc
    if( filebuff = (u_int*)loadfile(filename) ) {
        //printf("File readed OK\n");
        LoadTexture((u_long *)filebuff, timImage);
        //printf("Loaded texture\n");
        //HGL_free(filebuff); //TODO: we need to free the buffer
        //printf("Free buffer\n");
    } else {
        printf("Couldn't read file :(\n");
    }
}

void GetSprite(TIM_IMAGE *tim, SPRITE *sprite) {

    // Get tpage value
    sprite->tpage = getTPage(tim->mode&0x3, 0, 
        tim->prect->x, tim->prect->y);
        
    // Get CLUT value
    if( tim->mode & 0x8 ) {
        sprite->clut = getClut(tim->crect->x, tim->crect->y);
    }
    
    // Set sprite size
    sprite->w = tim->prect->w<<(2-tim->mode&0x3);
    sprite->h = tim->prect->h;
    
    // Set UV offset
    sprite->u = (tim->prect->x&0x3f)<<(2-tim->mode&0x3);
    sprite->v = tim->prect->y&0xff;
    
    // Set neutral color
    sprite->col.r = 128;
    sprite->col.g = 128;
    sprite->col.b = 128;
    
}


void replace_char(char* str, char find, char replace) {
    char *current_pos = strchr(str,find);
    for (char* p = current_pos; (current_pos = strchr(str, find)) != NULL; *current_pos = replace);
}

void to_uppercase(char * str) {
  char *s = str;
  while (*s) {
    *s = toupper((unsigned char) *s);
    s++;
  }
}

void GetSpriteFromDisc(char *filename, SPRITE *sprite) {
    TIM_IMAGE timImage;

    char discFilename[64];
    strcpy(discFilename, "\\");
    replace_char(filename, '/', '\\');
    to_uppercase(filename);
    strcat(discFilename, filename);
    strcat(discFilename, ".TIM;1");

    LoadTextureFromDisc(discFilename, &timImage);
    GetSprite(&timImage, sprite);
}

void GetSpriteFromMemory(u_char *data, SPRITE *sprite) {
    TIM_IMAGE timImage;
    LoadTexture((u_long *)data, &timImage);
    GetSprite(&timImage, sprite);
}

