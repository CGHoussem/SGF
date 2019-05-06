#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "struct_SGF.h"
#include "primitives.h"
#include "command_shell.h"
#include "save_load_SGF.h"
#include "constants.h"

void format_disk(Disk* disk){
	Inode inode_null, inode;
	char name[]="root";
	
	inode = mkdir(name,inode_null);
	disk->inodes = &inode;
	disk->inodes->dir_blocks->prev_block = NULL;
	disk->nb_inode = 1;
	disk->data_blocks = NULL;
	disk->dir_blocks = disk->inodes->dir_blocks;
	disk->nb_dir_blocks = 1;
	disk->dir_blocks->tab_index[1].inode = &inode; //change the inode of the directory ".."
	printf("The disk has been successuflly formatted!\n");
}

int save_disk(Disk disk){
	FILE *file = fopen(DISK_FILE_NAME, "wb");

	if (file == NULL){
		printf("File '%s' couldn't be opened!\n", DISK_FILE_NAME);
		return 0;
	}

	if (fwrite(&disk, sizeof(Disk), 1, file) <= 0)
		return 0;
		
	fclose(file);
	return 1;
}

int load_disk(Disk* disk){
	FILE *file = fopen(DISK_FILE_NAME, "rb");
	if (file == NULL){
		printf("File '%s' not found!\n", DISK_FILE_NAME);
		return 0;
	}
	if (fread(disk, sizeof(Disk), 1, file) <= 0)
		return 0;

	fclose(file);
	return 1;
}

Directory_block* allocation_tab_block_directory(int size){
	Directory_block* tab_block = NULL;
	
	tab_block=(Directory_block*) malloc(size*sizeof(Directory_block));
	tab_block->next_block = NULL;

	return tab_block;
}

void init_block_directory(Directory_block* block,Inode inode,Inode prev_inode){
	char name_dir[]=".";
	char name_dir_father[]="..";
	
	block->tab_index = allocation_index(2);
	
	strcpy(block->tab_index[0].name,name_dir);
	block->tab_index[0].inode=&inode;
	
	strcpy(block->tab_index[1].name,name_dir_father);
	block->tab_index[1].inode=&prev_inode;
	
	block->next_block=NULL;
}

Index* allocation_index(int size){
	Index* index=NULL;
	
	index=(Index*)malloc(size*sizeof(Index));
	
	return index;
}

void init_permissions(char permissions[9]){
	permissions[0]='r';
	permissions[1]='w';
	permissions[2]='x';
	permissions[3]='r';
	permissions[4]='-';
	permissions[5]='-';
	permissions[6]='r';
	permissions[7]='-';
	permissions[8]='-';
}

void free_inode(Disk* disk,Inode* inode, Inode* prev_inode){
	prev_inode->next_inode=inode->next_inode;
	
	if(inode->type == 3){ //directory
		free_block_directory(disk,inode->dir_blocks);
	} else {
		free_block_data(disk,inode->data_blocks);
	}
	
	free(inode);
	
	disk->nb_inode--;
}

void free_block_directory(Disk* disk, Directory_block* block){
	Directory_block* prev_block = search_prev_block(disk->dir_blocks, block, DIRECTORY_BLOCK);
	if (prev_block != NULL)
		prev_block->next_block=block->next_block;
	
	free(block->tab_index);
	free(block);
	
	disk->nb_dir_blocks--;
	
}

void free_block_data(Disk* disk, Data_block* block){
	Data_block* prev_block=search_prev_block(disk->data_blocks, block, DATA_BLOCK);
	prev_block->next_block=block->next_block;
	
	free(block);
	
	disk->nb_data_blocks--;	
}

void* search_prev_block(void* first_block, void* block, Block_type type){
	if (first_block == NULL){
		return NULL;
	}
	if (type == DIRECTORY_BLOCK){
		Directory_block* fb = (Directory_block*) first_block;
		Directory_block* b = (Directory_block*) block;
		if(fb->next_block==block){
			return fb;
		} else{
			return search_prev_block(fb->next_block,b, DIRECTORY_BLOCK);
		}
	} else {
		Data_block* fb = (Data_block*) first_block;
		Data_block* b = (Data_block*) block;
		if(fb->next_block==block){
			return fb;
		} else{
			return search_prev_block(fb->next_block,b, DATA_BLOCK);
		}
	}
}