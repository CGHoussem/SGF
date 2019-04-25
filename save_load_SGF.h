#ifndef SAVE_LOAD_SGF
#define SAVE_LOAD_SGF

#include "struct_SGF.h"

//prototypes

void format_disk(Disk* disk); //disk initialization
Inode mkdir(char* name,Inode prev_inode); //create a directory and return its inode
Block_directory* allocation_tab_block_directory(int size); //dynamic allocation
void init_block_directory(Block_directory* block,Inode inode,Inode prev_inode); //initialization of a block directory
Index* allocation_index(int size); //dynamic allocation
void init_permissions(char permissions[9]); //initialization of permissions

#endif
