#include "Kernel.h"
#include "DirectoryEntry.h"
#include "Stat.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <queue>
#include <vector>
#include "FileSystem.h"
#include <map>

using namespace std;

class q_entry {
  public:
    int ino;
    string name;
    q_entry(){}
    q_entry(int n, string s) {
        ino = n;
        name = s;
    }
};

void storeAllocatedBlocks() {

}

int main(int argc, char ** argv)
{
    char PROGRAM_NAME[8];
    strcpy(PROGRAM_NAME, "fsck");
    
    if(Kernel::initialize() == false)
    {
		cout << "Failed to initialized Kernel" << endl;
		Kernel::exit(1);
	}
    
    queue<q_entry> queue;
    map<int, char> allocated_blocks_list;
    
    queue.push(q_entry(FileSystem::ROOT_INDEX_NODE_NUMBER, "/"));
    q_entry current_entry;
    
    map<int, int> inode_map;

    FileSystem * fileSystem = Kernel::openFileSystems;
    
    while(!queue.empty()) {

        current_entry = queue.front();
        
        queue.pop();
        
        IndexNode currIndexNode;
        fileSystem->readIndexNode(&currIndexNode , current_entry.ino );

        if (currIndexNode.getIndirectBlock() != FileSystem::NOT_A_BLOCK) {
            allocated_blocks_list.insert(pair<int , char>( currIndexNode.getIndirectBlock() , 'A'));
        }

        int current_nlink_count = 0;
        int blockSize = fileSystem->getBlockSize();
        int blocks = (currIndexNode.getSize() + blockSize-1) / blockSize;
        for( int i = 0 ; i < blocks ; i ++ )
        {
            int address = currIndexNode.getBlockAddress(i, fileSystem) ;
            if( address != FileSystem::NOT_A_BLOCK ) {
                allocated_blocks_list.insert(pair<int , char>( address , 'A'));
            }
        }

        if( ( currIndexNode.getMode() & Kernel::S_IFMT ) != Kernel::S_IFDIR )	{
            // This is a file
            map<int,int>::iterator it = inode_map.find(current_entry.ino);

            if (it == inode_map.end()) {
                inode_map.insert (pair<int,int>(current_entry.ino,1) );
            } 
            else {
                it->second = it->second + 1;
            }
  	    } else {
            // This is a dir
            current_nlink_count = 0;
            FileDescriptor * fileDescriptor = new FileDescriptor(fileSystem, currIndexNode, Kernel::O_RDONLY);

            //	cout << fileDescriptor.getOffset()<< endl;
            int fd = Kernel::open(fileDescriptor);
            if( fd < 0 )
            {
                // process.errno = ???
                return -1 ;
            }

            // create a buffer for reading directory entries
            DirectoryEntry directoryEntry;
            int status = 0 ;

            // while there are more directory blocks to be read
            while( true )
            {
                // read a directory entry
                status = Kernel::readdir(fd, directoryEntry);
                // cout << directoryEntry.toString() << endl;

                if( status <= 0 )
                {
                    // cout << "here!!!" << endl;
                    // we got to the end of the directory, or
                    // encountered an error, so quit
                    break ;
                }

                // read the inode block
                IndexNode nextIndexNode;
                fileSystem->readIndexNode(&nextIndexNode , directoryEntry.getIno() ) ;

                if( ( nextIndexNode.getMode() & Kernel::S_IFMT ) == Kernel::S_IFDIR )	{
                    current_nlink_count++;
                }

                if (strcmp(directoryEntry.getName() , ".") != 0 && strcmp(directoryEntry.getName() , "..") != 0 ) {
                    queue.push(q_entry(directoryEntry.getIno(), directoryEntry.getName()));
                }
            }

            //check if the dic have correct nlink
            if(current_nlink_count != currIndexNode.getNlink()) {
                cout << "Directory : " << current_entry.name << " has incorrect nlinks -- Expected : " << currIndexNode.getNlink() << " Actual : " << current_nlink_count << endl;
            }
        }
    }

    // Checking nlinks for files
    for (map<int,int>::iterator it = inode_map.begin(); it != inode_map.end(); ++it) {
        IndexNode currIndexNode;
        fileSystem->readIndexNode(&currIndexNode , it->first );
        if(currIndexNode.getNlink() != it->second) {
            cout << "Inode Number : " << it->first << " has incorrect nlinks -- Expected : " << currIndexNode.getNlink() << " Actual : " << it->second << endl;
        }
    }

    fileSystem->checkBlocks(allocated_blocks_list);

	// exit with success if we process all the arguments
	Kernel::exit( 0 ) ;
}