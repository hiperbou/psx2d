#include <sys/types.h>	// This provides typedefs needed by libgte.h and libgpu.h
#include <libapi.h>     // API header, has InitPAD() and StartPAD() defs

#include "../input.h"



//Controller	Type Value
//Mouse	0x1
//Namco NegCon	0x2
//Digital pad or Dual-shock in digital mode (no light)	0x4
//Analog Stick or Dual-analog in green mode	0x5
//Dual-shock in analog mode or Dual-analog in red mode	0x7
//Namco JogCon	0xE
#define CONTROLLER_MOUSE 0x1
#define CONTROLLER_NAMCO_NEGCON 0x2
#define CONTROLLER_DIGITAL 0x4
#define CONTROLLER_ANALOG 0x5
#define CONTROLLER_DUAL_SHOCK 0x7
#define CONTROLLER_NAMCO_JOGCON 0xE

static u_char padbuff[2][34];

void initInput() {
        // Initialize the pads
    InitPAD( padbuff[0], 34, padbuff[1], 34 );
    padbuff[0][0] = padbuff[0][1] = 0xff;
    padbuff[1][0] = padbuff[1][1] = 0xff;
    StartPAD();
    //StopPAD() // only required when transferring execution to a child PS-EXE or another PS-EXE altogether.
}

PADSTATE * getPadState(int player) {
    return (PADSTATE*)padbuff[player];
}

uint16_t getButtons(int player) {
    PADSTATE *pad = (PADSTATE*)padbuff[player];
    if (pad->stat != 0) return 0;
    // Only parse when a digital pad, 
    // dual-analog and dual-shock is connected
    if(pad->type != CONTROLLER_DIGITAL && 
        pad->type != CONTROLLER_ANALOG && 
        pad->type != CONTROLLER_DUAL_SHOCK) return 0;

    return ~pad->btn;
}
