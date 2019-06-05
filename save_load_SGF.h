#ifndef SAVE_LOAD_SGF
#define SAVE_LOAD_SGF

#include "struct_SGF.h"

// Prototypes
// Code SGF Methods
void format_disk(Disk* disk); //disk initialization
int save_disk(Disk disk); // saves physically the state of the disk (return 0 if error ecountered)
int load_disk(Disk* disk); // loads the state of the saved disk (returns 0 if error encountered)
Inode* get_last_inode(Disk disk); //return the last inode of the list
void add_inode(Inode* inode, Disk* disk); //add an inode at the end of the list
Directory_block* get_last_dir_block(Disk disk); //return the last directory block of the list
void add_dir_block(Directory_block* dir_block, Disk* disk); //add a directory block at the end of the list
Data_block* get_last_data_block(Disk disk); //return the last data block of the list
void add_data_block(Data_block* dir_block, Disk* disk); //add a data block at the end of the list
void update_tab_index(Inode* current_inode, Inode* inode_to_add); //update the index of current_inode by adding inode_to_add
void remove_tab_index(Inode* inode_to_remove,Disk* disk); //remove the inode from the index of the parent directory
Inode* search_file_in_directory(char* file_name,Directory_block* directory); //return the inode of the given file name if exists, else return NULL
Inode* search_parent_inode(Inode* inode,Disk* disk); //return the parent inode of the given inode

// Allocations
Index* allocation_index(int size); //dynamic allocation
Directory_block* allocation_tab_block_directory(int size); //dynamic allocation of a directory block
Data_block* allocation_tab_block_data(int size); // dynamic allocation of a data block

//Free
void free_inode(Disk* disk,Inode* inode); //delete an inode
void free_block_directory(Disk* disk, Directory_block* block); //delete a block directory
void free_block_data(Disk* disk, Data_block* block); //delete a block data
void free_disk(Disk* disk); //delete the disk


// Initializations
void init_block_directory(Directory_block* block,Inode* inode_directory,Inode* inode_parent_directory,Disk* disk); //initialization of a directory block
void init_block_data(Data_block* block,Disk* disk); //initialization of a data block
void init_permissions(char permissions[9]); //initialization of permissions

#endif
