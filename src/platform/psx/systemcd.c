#include <sys/types.h>
#include <stdio.h>
#include <libcd.h>

#include "../../core/hgl_mem.h"


// CD specifics
#define CD_SECTOR_SIZE 2048
// Converting bytes to sectors SECTOR_SIZE is defined in words, aka int
#define BytesToSectors(len) ( ( len + CD_SECTOR_SIZE - 1 ) / CD_SECTOR_SIZE ) 

void initCD() {
    CdInit();
}

u_long *loadfile(char *filename)
{
    CdlFILE filePos;
    u_long    *buff  = NULL;
    
    // Those are not strictly needed, but we'll use them to see the commands results.
    // They could be replaced by a 0 in the various functions they're used with.
    u_char CtrlResult[8];
    // Value returned by CDread() - 1 is good, 0 is bad
    int CDreadOK = 0;
    // Value returned by CDsync() - Returns remaining sectors to load. 0 is good.
    int CDreadResult = 0;
    
    /* locate the file on the CD */
    if( CdSearchFile( &filePos,  filename ) == NULL )
    {
        /* print error message if file not found */
        printf( "%s not found.\n", filename );
    }
    else
    {
        /* calculate number of sectors to read for the file */
        //numsecs = (filePos.size+2047)/2048;
        //printf( "%i numsecs.\n", numsecs );
        
        /* allocate buffer for the file (replace with malloc3() for PsyQ) */
        //buff = (u_long*)malloc3( 2048*numsecs );
        //printf( "malloc3.\n" );

        // Allocate memory
        buff = HGL_malloc( BytesToSectors(filePos.size) * CD_SECTOR_SIZE );


        /* set read target to the file */
        //CdControl( CdlSetloc, (u_char*)&filePos.pos, 0 );
        //printf( "CdControl.\n" );
        /* start read operation */
        //CdRead( numsecs, (u_long*)buff, 0/*CdlModeSpeed*/ );
        //printf( "CdRead.\n" );
        /* wait until the read operation is complete */
        //CdReadSync( 0, 0 );
        //printf( "CdReadSync.\n" );

        // Issue  CdlSetloc CDROM command : Set the seek target position
        // Beware of a misnomed 'sector' member in the CdlLOC struct that should really be named 'frame'.
        // https://discord.com/channels/642647820683444236/663664210525290507/864912470996942910
        CdControl(CdlSetloc, (u_char *)&filePos.pos, CtrlResult);

        //CdReadFile(filename, buff, 0);
        //printf( "CdReadFile.\n" );
		//CdReadSync(0,0);
        //printf( "CdReadSync.\n" );
        CDreadOK = CdRead( (int)BytesToSectors(filePos.size), (u_long *)buff, CdlModeSpeed );
        //printf( "CdRead.\n" );
        // Wait for operation to complete
        CDreadResult = CdReadSync(0, 0);
        //printf( "CdReadSync.\n" );
        // Print the content of the loaded file - See HELO.DAT
        //printf("%s%d\n", (char *)buff, VSync(-1));
        // Print heap and buffer addresses
        //printf("Heap: %x - Buf: %x\n", ramAddr, dataBuffer);
        // Print returned values
        //printf("CdCtrl: %d\nRead  : %d %d\n", CtrlResult[0], CDreadOK, CDreadResult);
        // Print filesize in bytes/sectors
        //printf("Size: %dB sectors: %d", filePos.size, BytesToSectors(filePos.size));
    }
    
    return buff;
    
}