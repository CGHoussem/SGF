#ifndef SAVE_LOAD
#define SAVE_LOAD

#include "struct_SGF.h"

void save_disk(Disk disk);
void save_inode(Inode* inode);
void save_inodes(Inode* inodes);
void save_dir_block(Directory_block* dir_block);
void save_dir_blocks(Directory_block* dir_blocks);
void save_data_block(Data_block* data_block);
void save_data_blocks(Data_block* data_blocks);

Disk load_disk(Disk disk);
Inode load_inode(Inode* inode);
Inode* load_inodes(Inode* inodes);
Directory_block load_dir_block(Directory_block dir_block);
Directory_block* load_dir_blocks(Directory_block* dir_blocks);
Data_block load_data_block(Data_block data_block);
Data_block* load_data_blocks(Data_block* data_blocks);

#endif