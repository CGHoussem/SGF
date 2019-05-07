#ifndef SAVE_LOAD_SGF
#define SAVE_LOAD_SGF

#include "struct_SGF.h"

// Prototypes
// Code SGF Methods
void format_disk(Disk* disk); //disk initialization
int save_disk(Disk disk); // saves physically the state of the disk (return 0 if error ecountered)
int load_disk(Disk* disk); // loads the state of the saved disk (returns 0 if error encountered)
//void* search_prev_block(void* first_block, void* block, Block_type type); //return the previous block 

// Allocations
Index* allocation_index(int size); //dynamic allocation
Directory_block* allocation_tab_block_directory(int size); //dynamic allocation

//Free
void free_inode(Disk* disk,Inode* inode); //delete an inode
void free_block_directory(Disk* disk, Directory_block* block); //delete a block directory
void free_block_data(Disk* disk, Data_block* block); //delete a block data


// Initializations
void init_block_directory(Directory_block* block,Inode* inode,Inode* prev_inode); //initialization of a block directory
void init_permissions(char permissions[9]); //initialization of permissions

#endif
