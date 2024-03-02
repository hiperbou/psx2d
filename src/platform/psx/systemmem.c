#include <sys/types.h>
#include "hgl_mem.h"

//~ struct EXEC * exeStruct;
// Define start address of allocated memory
// Let's use an array so we don't have to worry about using a memory segment that's already in use.
static unsigned char ramAddr[0x40000]; // https://discord.com/channels/642647820683444236/663664210525290507/864936962199781387
// We could also set a memory address manually, but we have to make sure this won't FixedPool_get in the way of other routines.
// void * ramAddr = (void *)0x80030D40; 

/*extern unsigned long _bss_objend; //undefined reference to `_bss_objend'

void InitializeHeap()
{
	u_long stack = 0x801FFFF0; // default stack for this PS-EXE (as defined in SYSTEM.CNF)
    u_long _stacksize = 0x10000; // 64 KB (make this smaller for simple programs)
	u_long addr1, addr2;
	
	addr1 = (stack - _stacksize);
	//printf("addr1 = %X\n", (int)addr1);
	addr2 = (addr1 - (int)&_bss_objend);
	//printf("addr2 = %X\n", (int)addr2);
	
	//printf("\nUsing the end BSS address %X for InitHeap3\n", (int)&_bss_objend);
	//printf("Reserving %d bytes for InitHeap3...", (int)addr2);
	
	InitHeap3(&_bss_objend, addr2);
}*/

void initMemory() {
    HGL_InitHeap((u_long *)ramAddr, sizeof(ramAddr));
    //InitializeHeap();
}

