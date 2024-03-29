#include <sys/types.h>	// This provides typedefs needed by libgte.h and libgpu.h
#include <stdio.h>	
#include <libetc.h>	// Includes some functions that controls the display
#include <libgte.h>	// GTE header, not really used but libgpu.h depends on it
#include <libgpu.h>	// GPU library header
#include <libcd.h>
#include <stdbool.h>
#include <stddef.h>

//#include <string.h>
#include <strings.h>
#include <ctype.h>

#include "system.h"
#include "../core/hgl.h"
#include "../engine/fader.h"

// Define environment pairs and buffer counter
static DISPENV disp[2];
static DRAWENV draw[2];
static int db = 0;

#define ORDER_TABLE_LENGTH 8

static u_long ot[2][ORDER_TABLE_LENGTH];
static u_long *currentOrderTable;

static char pribuff[2][32768]; // Primitive buffer (32768)
static char *nextpri;          // Next primitive pointer

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

void setClearColor(uint8_t r, uint8_t g, uint8_t b) {
    setRGB0(&draw[0], r, g, b);
    setRGB0(&draw[1], r, g, b);
}

void initClearColor() {
    // Specifies the clear color of the DRAWENV
    //setRGB0(&draw[0], 175, 249, 240);
    //setRGB0(&draw[1], 175, 249, 240);
    setRGB0(&draw[0], 0, 0, 0);
    setRGB0(&draw[1], 0, 0, 0);
    // Enable background clear
    draw[0].isbg = 1;
    draw[1].isbg = 1;
}

void applyEnvironments() {
    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);
}

void clearOrderingTable() {
    //ClearOTag(ot[db], ORDER_TABLE_LENGTH);  // Clear ordering table
    ClearOTagR(currentOrderTable, ORDER_TABLE_LENGTH);  // Clear ordering table Reverse order
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

    currentOrderTable = ot[db];

    nextpri = pribuff[db]; // Set initial primitive pointer address

    clearOrderingTable();
}

void preloadFadePrimitives();

void display() {
    // Wait for GPU to finish drawing and V-Blank
    DrawSync(0);
    VSync(0);

    // Apply environments
    applyEnvironments();
    // Enable display
    SetDispMask(1);

    //DrawOTag(currentOrderTable);   // Draw the ordering table
    DrawOTag(currentOrderTable + ORDER_TABLE_LENGTH - 1);   // Draw the ordering table Reversed
    // Flip buffer counter
    db = !db;
    currentOrderTable = ot[db];

    nextpri = pribuff[db];      // Reset next primitive pointer

    clearOrderingTable();

    preloadFadePrimitives();
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

void addPrimitive(int x, int y, int z, int w, int h, int r, int g, int b) {
    TILE * tile = (TILE*)nextpri;      // Cast next primitive

    setTile(tile);              // Initialize the primitive (very important)
    setXY0(tile, x, y);       // Set primitive (x,y) position
    setWH(tile, w, h);        // Set primitive size
    setRGB0(tile, r, g, b); // Set color yellow
    addPrim(&currentOrderTable[z], tile);      // Add primitive to the ordering table

    nextpri += sizeof(TILE);    // Advance the next primitive pointer
}

void addSpriteTim(int x, int y, int z, int w, int h, int tim_uoffs, int tim_voffs, RECT tim_crect) {
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
    addPrim(&currentOrderTable[z], sprt);          // Sort primitive to OT

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

void addSprite(int x, int y, int z, SPRITE *sprite, int uploadTpage) {
    nextpri = SortSprite(x, y, &currentOrderTable[z], nextpri, sprite, uploadTpage);
}

static inline void SortTile16(int x, int y, u_long *ot, SPRITE *sprite) {
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

static inline void SortTile8(int x, int y, u_long *ot, SPRITE *sprite) {
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

void addTile16(int x, int y, int z, SPRITE *sprite) {
    SortTile16(x, y, &currentOrderTable[z], sprite);
}

void addTile8(int x, int y, int z, SPRITE *sprite) {
    SortTile8(x, y, &currentOrderTable[z], sprite);
}

void sortRotSprite(int x, int y, int angle, int scale, int flags, SPRITE *sprite, int uploadTpage, u_long *orderTable)
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

    SVECTOR	v[4];

    if(angle == 0) {
        v[0].vx = left + x;
        v[0].vy = top + y;

        v[1].vx = right + x;
        v[1].vy = v[0].vy;

        v[2].vx = v[0].vx;
        v[2].vy = bottom + y;

        v[3].vx = v[1].vx;
        v[3].vy = v[2].vy;
    } else {
        SVECTOR	s[4];

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
    int mirroredVertical = flags & 2;

    int spriteWidthMinusOne = sprite->w - 1;
    int spriteHeightMinusOne = sprite->h - 1;

    int uu = sprite->u + spriteWidthMinusOne * mirroredHorizontal;
    int vv = sprite->v + spriteHeightMinusOne * mirroredVertical;
    int ww = mirroredHorizontal ? -spriteWidthMinusOne : spriteWidth;
    int hh = mirroredVertical ? -spriteHeightMinusOne : spriteHeight;
    setUVWH( quad, uu, vv, ww, hh);

    addPrim(orderTable, quad );
    nextpri += sizeof(POLY_FT4);

    //TODO: check if we are already in the same page to optimize
    if (!uploadTpage /*last_tpage == sprite->tpage*/) {

    } else {
        last_tpage = sprite->tpage;

        DR_TPAGE *tpage = (DR_TPAGE*)nextpri;             // Sort the texture page value
        setDrawTPage(tpage, 0, 1, sprite->tpage);
        addPrim(orderTable, tpage);

        nextpri += sizeof(DR_TPAGE);    // (set to nextpri)
    }
}

void addRotSprite( int x, int y, int z, int angle, int scale, int flags, SPRITE *sprite, int uploadTpage) {
    sortRotSprite(x, y, angle, scale, flags, sprite, uploadTpage, &currentOrderTable[z]);
}

void sortTpage(u_short newTpage, int z) {
    DR_TPAGE *tpage = (DR_TPAGE*)nextpri;             // Sort the texture page value
    setDrawTPage(tpage, 0, 1, newTpage);
    addPrim(&currentOrderTable[z], tpage);

    nextpri += sizeof(DR_TPAGE);    // (set to nextpri)
}

//deprecated
void setTexturePage(int tim_mode, RECT tim_prect, RECT tim_crect) {
    draw[0].tpage = getTPage( tim_mode&0x3, 0, tim_prect.x, tim_prect.y );
    draw[1].tpage = getTPage( tim_mode&0x3, 0, tim_prect.x, tim_prect.y );
}

void addTexturePage(int tim_mode, RECT tim_prect, int z) {
    DR_TPAGE *tpage = (DR_TPAGE*)nextpri;
    setDrawTPage(tpage, 0, 1,       // Set TPage primitive
    getTPage(tim_mode & 0x3, 0, 
        tim_prect.x, tim_prect.y));

    addPrim(&currentOrderTable[z], tpage);         // Sort primitive to OT

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



/*
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
*/


static TILE * fadePrimitiveTile;
static DR_MODE *fadePrimitiveDrMode;

static void addFadePrimitive(int z, int screenWidth, int screenHeight, int fade, enum FadeColor fadeColor) {
    TILE * tile = (TILE*)nextpri;
    setTile(tile);
    setXY0(tile, 0, 0);
    setWH(tile, screenWidth, screenHeight);
    setRGB0(tile, fade, fade, fade);
    setSemiTrans(tile, 1);
    addPrim(&currentOrderTable[z], tile);
    nextpri += sizeof(TILE);

    DR_MODE *drMode = (DR_MODE*)nextpri;
    setDrawMode(drMode, 0, 0, getTPage(0, fadeColor, 0, 0), 0);
    addPrim(&currentOrderTable[z], drMode);
    nextpri += sizeof(DR_MODE);

    fadePrimitiveTile  = tile;
    fadePrimitiveDrMode = drMode;
}

static void updateFadePrimitive(int fade, enum FadeColor fadeColor) {
    setRGB0(fadePrimitiveTile, fade, fade, fade);
    setDrawMode(fadePrimitiveDrMode, 0, 0, getTPage(0, fadeColor, 0, 0), 0);
}

inline void preloadFadePrimitives() {
    addFadePrimitive(0, 320, 240, 0, BlackFadeColor);
}

void fadeToBlack(uint8_t fade) {
    updateFadePrimitive(fade, BlackFadeColor);
}

void fadeToWhite(uint8_t fade) {
    updateFadePrimitive(fade, WhiteFadeColor);
}