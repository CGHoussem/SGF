#ifndef UTILITY
#define UTILITY

#include "struct_SGF.h"

#define DEBUG 1

// Macros
#define print_info(msg, ...) printf(BOLDCYAN msg "\n"RESET, ##__VA_ARGS__);
#define print_error(msg, ...) printf(BOLDRED msg "\n"RESET, ##__VA_ARGS__);

#define print_debug(msg, ...) if (DEBUG) fprintf(stderr, HIGHTLIGHT "[DEBUG] %s:%d:%s(): " msg RESET"\n", __FILE__, __LINE__, __func__, __VA_ARGS__);


// Prototypes
// Code SGF Methods
void format_disk(Disk* disk); //disk initialization
// int save_disk(Disk* disk); // saves physically the state of the disk (return 0 if error encountered)
// int load_disk(Disk* disk); // loads the state of the saved disk (returns 0 if error encountered)
Inode* get_last_inode(Disk disk); //return the last inode of the list
void add_inode(Inode* inode, Disk* disk); //add an inode at the end of the list
DataBlock* get_last_inode_data_block(Inode* inode); // retturns the last data block of an inode
DirectoryBlock* get_last_dir_block(Disk disk); //return the last directory block of the list
DataBlock* get_last_data_block(Disk disk); //return the last data block of the list
void update_tab_index(Inode* current_inode, Inode* inode_to_add); //update the index of current_inode by adding inode_to_add
void remove_tab_index(Inode* inode_to_remove,Inode* parent_inode,Disk* disk); //remove the inode from the index of the parent directory
Inode* search_file_in_directory(char* file_name,DirectoryBlock* directory); //return the inode of the given file name if exists, else return NULL
Inode* search_parent_inode(Inode* inode,Disk* disk); //return the parent inode of the given inode

// Allocations
Index* allocation_index(int size); //dynamic allocation
DirectoryBlock* allocation_block_directory(); //dynamic allocation of a directory block
DataBlock* allocation_block_data(); //dynamic allocation of a data block
DataBlock* allocation_tab_block_data(int size); // dynamic allocation of an array data block

//Free
void free_inode(Inode* inode); //delete an inode
void free_dirblock(DirectoryBlock* block); //delete a block directory
void free_datablock(DataBlock* block); //delete a block data
void free_disk(Disk* disk); //delete the disk

// Initializations
void init_block_directory(DirectoryBlock* block,Inode* inode_directory,Inode* inode_parent_directory,Disk* disk); //initialization of a directory block
DataBlock* init_datablock(); //initialization of a data block
void init_permissions(char permissions[9]); //initialization of permissions
void delete_permissions(char permissions[9]); //deletion of permissions

// Counting
unsigned int count_inodes(Disk disk);
unsigned int count_disk_datablocks(Disk disk);
unsigned int count_disk_dirblocks(Disk disk);
unsigned int count_inode_datablocks(Inode *inode);
unsigned int count_inode_dirblocks(Inode inode);

// String Manipiulations
char* substring(char* string, int position, int length);

#endif
