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
	char name[MAX_FILE_NAME];
	Inode* inode;
};

enum Block_type {
	DIRECTORY_BLOCK,
	DATA_BLOCK
};

struct Directory_block {
	Directory_block* prev_block;
	Directory_block* next_block;
	
	Index* tab_index;
};

struct Data_block {
	Data_block* prev_block;
	Data_block* next_block;

	char data[BUFFER_SIZE];
	int size;
};

struct Inode {
	char name[MAX_FILE_NAME];
	char permissions[9]; // rwxr--r--
	int type; // 1 = text, 2 = binary, 3 = directory
	time_t date_creation;
	time_t date_modification;
	Directory_block* dir_blocks;
	Data_block* data_blocks;
	Inode* next_inode;
};

struct Disk {
	Inode* inodes;	// 1st inode
	Directory_block* dir_blocks; // list of directory blocks
	Data_block* data_blocks; // list of data blocks
	
	int nb_inode; // count of inodes
	int nb_dir_blocks; // count of directory blocks
	int nb_data_blocks; // count of data blocks
};

#endif
