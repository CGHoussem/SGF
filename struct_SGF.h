#ifndef STRUCT_SGF
#define STRUCT_SGF

#include "constants.h"
#include <time.h>

typedef struct Index Index;
typedef enum BlockType BlockType;
typedef struct DirectoryBlock DirectoryBlock;
typedef struct DataBlock DataBlock;
typedef struct Inode Inode;
typedef struct Disk Disk;

// This is unecessarry (should be replaced by a simple inode pointer)
// TODO: rework
struct Index {
	char name[MAX_FILE_NAME];	// name of the index
	Inode* inode; 				// inode pointed by the index

	Index *next_index;			// pointer to next index in the list
};

enum BlockType {
	DIRECTORY_BLOCK,
	DATA_BLOCK
};

// Useless to be a list since we have indexes!
struct DirectoryBlock {
	Index* indexes;				// list of indexes

	DirectoryBlock* next_block;	// next block
};

struct DataBlock {
	char data[BUFFER_SIZE]; 	// content of the data block

	DataBlock* next_block;		// next block
};

struct Inode {
	unsigned long uid;

	char name[MAX_FILE_NAME]; 		// name of the file
	char permissions[10]; 			// rwxr--r--
	int type; 						// 1: text, 2: binary, 3: directory
	time_t creation_date; 			// date of creation
	time_t modification_date; 		// last date of modification
	DataBlock* datablocks;			// the content of the file (if it is a file)
	DirectoryBlock* dirblock;		// the available directories (if it is a directory)
	
	Inode* next_inode; 				// next inode in a list
};

struct Disk {
	Inode* inodes;					// doubly list of inodes
};

#endif
