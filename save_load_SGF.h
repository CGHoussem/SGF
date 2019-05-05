#ifndef SAVE_LOAD_SGF
#define SAVE_LOAD_SGF

#include "struct_SGF.h"

// Prototypes
// Code SGF Methods
void format_disk(Disk* disk); //disk initialization
int save_disk(Disk disk); // saves physically the state of the disk (return 0 if error ecountered)
int load_disk(Disk* disk); // loads the state of the saved disk (returns 0 if error encountered)
Block* search_prev_block(Block* first_block,Block* block); //return the previous block 

// Allocations
Index* allocation_index(int size); //dynamic allocation
Block_directory* allocation_tab_block_directory(int size); //dynamic allocation

//Free
void free_inode(Disk* disk,Inode* inode, Inode* prev_inode); //delete an inode
void free_block_directory(Disk* disk, Block* block); //delete a block directory
void free_block_data(Disk* disk, Block* block); //delete a block data


// Initializations
void init_block_directory(Block_directory* block,Inode inode,Inode prev_inode); //initialization of a block directory
void init_permissions(char permissions[9]); //initialization of permissions

// Commands & Primitives
Inode mkdir(char* name,Inode prev_inode); //create a directory and return its inode

#endif
