#ifndef STRUCT_SGF
#define STRUCT_SGF

#include "constants.h"
#include <time.h>

typedef struct Index Index;
typedef enum Block_type Block_type;
typedef struct Directory_block Directory_block;
typedef struct Data_block Data_block;
typedef struct Inode Inode;
typedef struct Disk Disk;

struct Index {
	char name[MAX_FILE_NAME];	// name of the index
	Inode* inode; 				// inode pointed by the index
};

enum Block_type {
	DIRECTORY_BLOCK,
	DATA_BLOCK
};

struct Directory_block {
	Index* tab_index; 				// array of indexes
	int nb_index; 					// number of indexes

	Directory_block* prev_block; 	// previous block in a list
	Directory_block* next_block; 	// next block in a list
};

struct Data_block {
	char data[BUFFER_SIZE]; 	// content of the data block
	int size; 					// number of chars contained in data

	Data_block* prev_block; 	// previous block in a list
	Data_block* next_block; 	// next block in a list
};

struct Inode {
	char name[MAX_FILE_NAME]; 		// name of the file
	char permissions[10]; 			// rwxr--r--
	int type; 						// 1: text, 2: binary, 3: directory
	time_t date_creation; 			// date of creation
	time_t date_modification; 		// last date of modification
	Directory_block* dir_blocks;	// directory block associated
	Data_block** data_blocks; 		// array of data blocks allocated for this inode
	
	int nb_data_blocks; 			// number of data blocks allocated for this inode
	int nb_links; 					// number of symbolic links related to this inode
	
	Inode* prev_inode;				// previous inode in a list
	Inode* next_inode; 				// next inode in a list
};

struct Disk {
	Inode* inodes;					// doubly list of inodes
	Directory_block* dir_blocks;	// doubly list of directory blocks
	Data_block* data_blocks; 		// doubly list of data blocks
	
	int nb_inode; 					// number of inodes
	int nb_dir_blocks; 				// number of directory blocks
	int nb_data_blocks; 			// number of data blocks
};

#endif
