#ifndef SAVE_LOAD_SGF
#define SAVE_LOAD_SGF

#include "struct_SGF.h"

// Prototypes
void format_disk(Disk* disk); // disk initialization
int file_exists(char* filename); // checks if there is file that exists with a give filename
int disk_exists(); // checks if the disk file exists or not

// Save Functions
int save_disk(Disk disk); // physically saves the state of the disk (return 0 if error ecountered)
int save_inode_data_blocks(int inode_index, Data_block* blocks); // physically saves a list of data blocks in a temporary file
int save_inode_dir_blocks(int inode_index, Directory_block* blocks); // physcially saves a list of directory blocks in a temporary file
int save_data_blocks(Data_block* blocks); // physically saves a list of data blocks in a temporary file
int save_dir_blocks(Directory_block* blocks); // physcially saves a list of directory blocks in a temporary file
int save_inodes(Inode* inodes); // physcailly saves a list of inodes in a temporary file

// Load Functions
int load_disk(Disk* disk); // loads the state of the saved disk (returns 0 if error encountered)
int load_data_blocks(Data_block* blocks, int nb_data_blocks); // loads the data blocks
int load_dir_blocks(Directory_block* blocks, int nb_dir_blocks); // loads the directory blocks
int load_inode_data_blocks(int inode_index, Data_block* blocks, int nb_data_blocks); // loads an inode's data blocks
int load_inode_dir_blocks(Inode* inode, int inode_index, Directory_block* blocks); // loads an inode's directory blocks
int load_inodes(Inode* inodes, int nb_inodes); // loads the saved inodes

// Extra Functions needed for Save/Load
void addDataBlockTail(Data_block* head, Data_block block);
void addDirBlockTail(Directory_block* head, Directory_block* block);
void addInodeTail(Inode* head, Inode inode);
int read_index(Inode* inode, FILE* f, Index* index);
int read_data_block(FILE* f, Data_block* block);
int read_dir_block(FILE* f, Directory_block* block, int dir_index);
int read_inode_dir_block(Inode* inode, FILE* f, Directory_block* block, int inode_index, int dir_index);
int read_inode(FILE* f, int inode_index, Inode* inode);

Inode* get_last_inode(Disk disk); //return the last inode of the list
Directory_block* get_last_dir_block(Disk disk); //return the last directory block of the list
Data_block* get_last_data_block(Disk disk); //return the last data block of the list

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

// Free
void free_inode(Disk* disk,Inode* inode); //delete an inode
void free_block_directory(Disk* disk, Directory_block* block); //delete a block directory
void free_block_data(Disk* disk, Data_block* block); //delete a block data
void free_disk(Disk* disk); //delete the disk


// Initializations
void init_block_directory(Directory_block* block,Inode* inode_directory,Inode* inode_parent_directory,Disk* disk); //initialization of a directory block
void init_block_data(Data_block* block,Disk* disk); //initialization of a data block
void init_permissions(char permissions[9]); //initialization of permissions
void delete_permissions(char permissions[9]); //deletion of permissions


#endif
