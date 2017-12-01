#include "Kernel.h"
#include "DirectoryEntry.h"
#include "Stat.h"
#include <stdlib.h>
#include <string.h>

#define OUTPUT_MODE 0700

int main(int argc, char ** argv)
{
    char PROGRAM_NAME[8];
    strcpy(PROGRAM_NAME, "ln");

    char fromName[64];  // File of which link is to be created
    char toName[64];    // Target file which would be the link

    memset(fromName, '\0', 64);
    memset(toName, '\0', 64);

    // initialize the file system simulator kernel
    if(Kernel::initialize() == false)
    {
        cout << "Failed to initialized Kernel" << endl;
        Kernel::exit(1);
    }

    // print a helpful message if no command line arguments are given
    if(argc != 3) {
        cout << PROGRAM_NAME << ": Arguments mismatch" << endl;
        Kernel::exit(1);
    }

    strcpy(fromName, argv[1]);      // TODO :: This needs to a file
    strcpy(toName, argv[2]);        // TODO :: This needs to a directory


    // Operations for fromName  ##################################################
    int statusFrom = 0 ;
    // stat the name to get information about the file
    Stat fromStat;
    statusFrom = Kernel::stat( fromName , fromStat ) ;

    if( statusFrom < 0 )
    {
        Kernel::perror( PROGRAM_NAME ) ;
        Kernel::exit( 1 ) ;
    }

    short type = (short)( fromStat.getMode() & Kernel::S_IFMT ) ;

    // if not regular file, print the info
    if( type != Kernel::S_IFREG )
    {
        cout << "Only a file can be linked" << endl;
        Kernel::exit(1);
    }

    // ############################################################################





    char directoryEntryBuffer[DirectoryEntry::DIRECTORY_ENTRY_SIZE];
    memset(directoryEntryBuffer, '\0', DirectoryEntry::DIRECTORY_ENTRY_SIZE);

