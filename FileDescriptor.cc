#include "FileDescriptor.h"
#include "Kernel.h"
#include <stdlib.h>
#include <string.h>

FileDescriptor::FileDescriptor(FileSystem * newFileSystem, IndexNode &newIndexNode, int newFlags)
{
	deviceNumber = -1 ;
	indexNodeNumber = -1 ;
	offset = 0 ;
	
	flags = newFlags ;
	fileSystem = newFileSystem;
	
	//copy index node info
	newIndexNode.copy(indexNode);
	bytes = new char[fileSystem->getBlockSize()];
	memset(bytes, '\0', fileSystem->getBlockSize());
}

FileDescriptor::~FileDescriptor()
{
	if(bytes != NULL)
	{
		delete[] bytes;
	}
}

void FileDescriptor::setDeviceNumber(short newDeviceNumber)
{
	deviceNumber = newDeviceNumber;
}

short FileDescriptor::getDeviceNumber()
{
	return deviceNumber;
}

IndexNode * FileDescriptor::getIndexNode()
{
	return &indexNode;
}

void FileDescriptor::setIndexNodeNumber(short newIndexNodeNumber)
{
	indexNodeNumber = newIndexNodeNumber;
}

short FileDescriptor::getIndexNodeNumber()
{
	return indexNodeNumber;
}

int FileDescriptor::getFlags()
{
	return flags;
}

char * FileDescriptor::getBytes()
{
	return bytes;
}

short FileDescriptor::getMode()
{
	return indexNode.getMode();
}

int FileDescriptor::getSize()
{
	return indexNode.getSize();
}

void FileDescriptor::setSize(int newSize)
{
	indexNode.setSize(newSize);

	// write the inode
	fileSystem->writeIndexNode(&indexNode, indexNodeNumber);
}

short FileDescriptor::getBlockSize()
{
	return fileSystem->getBlockSize();
}

int FileDescriptor::getOffset()
{
	return offset;
}

void FileDescriptor::setOffset(int newOffset)
{
	offset = newOffset;
}

int FileDescriptor::readBlock(short relativeBlockNumber) 
{
	if(relativeBlockNumber > IndexNode::MAX_DIRECT_BLOCKS + (getBlockSize() / 3) )
	{
		Kernel::setErrno(Kernel::EFBIG);
		return -1 ;
	}

	// ask the IndexNode for the actual block number 
	// given the relative block number
	int blockOffset = indexNode.getBlockAddress(relativeBlockNumber, (void *) fileSystem);
	int blockSize = fileSystem->getBlockSize();

	if(blockOffset == FileSystem::NOT_A_BLOCK)
	{
		// clear the bytes if it's a block that was never written
		for(int i=0;i<blockSize;i++)
		{
			bytes[i] = (char)0 ;
		}
	}
	else
	{
		memset(bytes, '\0', blockSize);
		// read the actual block into bytes
		fileSystem->read(bytes, fileSystem->getDataBlockOffset() + blockOffset);
	}
	return 0 ;
}

int FileDescriptor::writeBlock(short relativeBlockNumber) 
{
	if(relativeBlockNumber > IndexNode::MAX_DIRECT_BLOCKS + (getBlockSize() / 3))
	{
		Kernel::setErrno( Kernel::EFBIG ) ;
		return -1 ;
	}

	// If there is no indirect block 
	if ( relativeBlockNumber > IndexNode::MAX_DIRECT_BLOCKS - 1
		 && indexNode.getIndirectBlock() == FileSystem::NOT_A_BLOCK){

			 cout << "Allocating indirect block" << endl;

			int blockOffset = fileSystem->allocateBlock() ;
			if( blockOffset < 0 )
			{
				return -1 ;
			}
			indexNode.setIndirectBlock(blockOffset);

			// TODO: Fix this
			for (int i = IndexNode::MAX_DIRECT_BLOCKS ; i < (IndexNode::MAX_DIRECT_BLOCKS + (getBlockSize() / 3) + 1) ; i++) {
				indexNode.setBlockAddress(i, FileSystem::NOT_A_BLOCK, (void*) fileSystem);
			}
			fileSystem->writeIndexNode(&indexNode, indexNodeNumber);
	}

	// ask the IndexNode for the actual block number 
	// given the relative block number
	int blockOffset = indexNode.getBlockAddress(relativeBlockNumber, (void*) fileSystem);

	if(blockOffset == FileSystem::NOT_A_BLOCK)
	{
		cout << "Inside assigning block " << relativeBlockNumber << endl;
		// allocate a block; quit if we can't
		blockOffset = fileSystem->allocateBlock() ;
		if( blockOffset < 0 )
		{
			return -1 ;
		}

		// update the inode
		indexNode.setBlockAddress(relativeBlockNumber, blockOffset, (void*) fileSystem);
		// write the inode
		fileSystem->writeIndexNode(&indexNode, indexNodeNumber);
	}
		
		
		cout << "Writing Bytes :: " ;
		for (int i = 0  ; i < fileSystem->getBlockSize() ; i+= 1) {
			cout << bytes[i];
		}
		cout << endl;

	// write the actual block from bytes
	fileSystem->write(bytes, fileSystem->getDataBlockOffset() + blockOffset);

	return 0 ;
}
