#ifndef STRUCT_SGF
#define STRUCT_SGF

#include <time.h>

typedef struct index index;
typedef struct block_data block_data;
typedef struct block_directory block_directory;
typedef union block block;
typedef struct inode inode;
typedef struct disk disk;

struct index {
	char* name;
	inode* p_inode;
};

struct block_data {
	char data[1024];
	int size;
};

struct bloc_directory {
	index* tab_index;
};

union block {
	block_data b_data;
	block_directory b_directory;
};

struct inode {
	char permissions[9]; // rwxr--r-- {1,1,1,1,0,0,1,0,0}
	int type; // 1 = text, 2 = binary, 3 = directory
	time_t date_creation;
	time_t date_modification;
	block p_block;
	inode* next_inode;
};

struct disk {
	inode* inodes;	//1st inode
	int nb_inode;
	block* blocks; //1st block
	int nb_block;
};

#endif
