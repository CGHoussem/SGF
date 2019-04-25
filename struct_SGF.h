#ifndef STRUCT_SGF
#define STRUCT_SGF

#include <time.h>

typedef struct Index Index;
typedef struct Block_data Block_data;
typedef struct Block_directory Block_directory;
typedef union Block Block;
typedef struct Inode Inode;
typedef struct Disk Disk;

struct Index {
	char name[20];
	Inode* inode;
};

struct Block_data {
	char data[1024];
	int size;
	Block* next_block;
};

struct Block_directory {
	Index* tab_index;
	Block* next_block;
};

union Block {
	Block_data* b_data;
	Block_directory* b_directory;
};

struct Inode {
	char name[20];
	char permissions[9]; // rwxr--r-- {1,1,1,1,0,0,1,0,0}
	int type; // 1 = text, 2 = binary, 3 = directory
	time_t date_creation;
	time_t date_modification;
	Block* tab_block;
	Inode* next_inode;
};

struct Disk {
	Inode* inodes;	//1st inode
	int nb_inode;
	Block* blocks; //1st block
	int nb_block;
};

#endif
