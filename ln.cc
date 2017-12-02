#include "Kernel.h"
#include "DirectoryEntry.h"
#include "Stat.h"
#include <stdlib.h>
#include <string.h>

#define OUTPUT_MODE 0700

int main(int argc, char ** argv) {

    char PROGRAM_NAME[8];
    strcpy(PROGRAM_NAME, "ln");
    
    if(argc != 3)
    {
        cout << PROGRAM_NAME << "Arguments mismatch" << endl;
        Kernel::exit( 1 ) ;
    }

    char fromName[512];  // File of which link is to be created
    char toName[512];    // Target file which would be the link

    memset(fromName, '\0', 512);
    memset(toName, '\0', 512);

    // initialize the file system simulator kernel
    if (Kernel::initialize() == false) {
        cout << "Failed to initialized Kernel" << endl;
        Kernel::exit(1);
    }

    strcpy(fromName, argv[1]);
    strcpy(toName, argv[2]);


    int status = 0;
    status = Kernel::link(fromName, toName);

    if (status < 0) {
        cout << "Failed to link files" << endl;
        Kernel::perror(PROGRAM_NAME);
        Kernel::exit(1);
    }

    Kernel::exit( 0 ) ;

    return 0;
}