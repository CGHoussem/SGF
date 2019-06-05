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
	
	disk->inodes = NULL;
	disk->dir_blocks = NULL;
	disk->data_blocks = NULL;
	
	disk->nb_inode = 0;
	disk->nb_dir_blocks = 0;
	disk->nb_data_blocks = 0;
	
	mkdir("root",disk,NULL);

	printf("The disk has been successfully formatted!\n");
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

Data_block* allocation_tab_block_data(int size){
	Data_block* tab_block = NULL;
	
	tab_block=(Data_block*) malloc(size*sizeof(Data_block));
	tab_block->next_block = NULL;

	return tab_block;
}

void init_block_directory(Directory_block* block,Inode* inode_directory,Inode* inode_parent_directory,Disk* disk){
	char name_parent_dir[] = "..";
	char name_dir[] = ".";
	
	block->tab_index = NULL;
	block->tab_index = allocation_index(2);
	block->nb_index = 2;
	
	strcpy(block->tab_index[0].name,name_dir);
	block->tab_index[0].inode=inode_directory;
	
	strcpy(block->tab_index[1].name,name_parent_dir);
	block->tab_index[1].inode=inode_parent_directory;
	
	block->next_block = NULL;
	add_dir_block(block,disk);

}

void init_block_data(Data_block* block,Disk* disk){

	block->size = 0;
	
	block->next_block = NULL;
	add_data_block(block,disk);

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

void free_inode(Disk* disk,Inode* inode){
	if(inode->prev_inode != NULL) {
		inode->prev_inode->next_inode=inode->next_inode;
	}
	if(inode->next_inode != NULL) {
		inode->next_inode->prev_inode = inode->prev_inode;
	}


	if(inode->type == DIRECTORY){ 
		free_block_directory(disk,inode->dir_blocks);
		inode->dir_blocks = NULL;
	} else {
		free_block_data(disk,inode->data_blocks);
		inode->data_blocks = NULL;
	}
	
	inode->prev_inode = NULL;
	inode->next_inode = NULL;
	
	free(inode);
	
	disk->nb_inode--;
	
}

void free_block_directory(Disk* disk, Directory_block* block){

	if (block->prev_block != NULL) {
		block->prev_block->next_block=block->next_block;
	}
	if(block->next_block != NULL) {
		block->next_block->prev_block = block->prev_block;
	}
	
	free(block->tab_index);
	free(block);
	
	disk->nb_dir_blocks--;

}

void free_block_data(Disk* disk, Data_block* block){
	
	if(block->prev_block != NULL) {
		block->prev_block->next_block=block->next_block;
	}
	if(block->next_block != NULL) {
		block->next_block->prev_block = block->prev_block;
	}
	
	free(block);
	
	disk->nb_data_blocks--;	
}

void free_disk(Disk* disk){
	if(disk->inodes != NULL){
		Inode* current_inode = disk->inodes;
		
		while(current_inode->next_inode != NULL){
			current_inode = current_inode->next_inode;
			free_inode(disk,current_inode->prev_inode);
		}
		free_inode(disk,current_inode);
	}
}

Inode* get_last_inode(Disk disk){
	Inode* current_inode = disk.inodes;
	
	while(current_inode->next_inode != NULL){
		current_inode = current_inode->next_inode;
	}
	
	return current_inode;
}

void add_inode(Inode* inode, Disk* disk){
	if(disk->inodes != NULL){
		Inode* last_inode = get_last_inode(*disk);
		
		last_inode->next_inode = inode;
		inode->prev_inode = last_inode;
		
		disk->nb_inode++;
	}
	else{
		disk->inodes = inode;
		inode->prev_inode = NULL;
		disk->nb_inode = 1;
	}
	
}

Directory_block* get_last_dir_block(Disk disk){
	Directory_block* current_dir_block = disk.dir_blocks;
	
	while(current_dir_block->next_block != NULL){
		current_dir_block = current_dir_block->next_block;
	}
	
	return current_dir_block;
}

void add_dir_block(Directory_block* dir_block, Disk* disk){
	if(disk->dir_blocks != NULL){
		Directory_block* last_dir_block = get_last_dir_block(*disk);
		
		last_dir_block->next_block = dir_block;
		dir_block->prev_block = last_dir_block;
		
		disk->nb_dir_blocks++;
	}
	else{
		disk->dir_blocks = dir_block;
		dir_block->prev_block = NULL;
		disk->nb_dir_blocks = 1;
	}
	
}
	
Data_block* get_last_data_block(Disk disk){
	Data_block* current_data_block = disk.data_blocks;
	
	while(current_data_block->next_block != NULL){
		current_data_block = current_data_block->next_block;
	}
	
	return current_data_block;
}

void add_data_block(Data_block* data_block, Disk* disk){
	if(disk->data_blocks != NULL){
		Data_block* last_data_block = get_last_data_block(*disk);
		
		last_data_block->next_block = data_block;
		data_block->prev_block = last_data_block;
		
		disk->nb_data_blocks++;
	}
	else{
		disk->data_blocks = data_block;
		data_block->prev_block = NULL;
		disk->nb_data_blocks = 1;
	}
	
}

void update_tab_index(Inode* current_inode, Inode* inode_to_add){
	Index* new_index = NULL;
	int i;
	
	new_index = allocation_index(current_inode->dir_blocks->nb_index+1);
	
	for(i=0;i<current_inode->dir_blocks->nb_index;i++){
		new_index[i] = current_inode->dir_blocks->tab_index[i]; //copy the old index in the new one
	}
	
	//add the directory at the end of the index
	strcpy(new_index[current_inode->dir_blocks->nb_index].name,inode_to_add->name);
	new_index[current_inode->dir_blocks->nb_index].inode = inode_to_add;
	
	free(current_inode->dir_blocks->tab_index);
	current_inode->dir_blocks->tab_index = new_index;
	
	current_inode->dir_blocks->nb_index++;
}

void remove_tab_index(Inode* inode_to_remove,Disk* disk){
	Index* new_index = NULL;
	int i;
	Inode* parent_inode = search_parent_inode(inode_to_remove,disk);
	
	new_index = allocation_index(parent_inode->dir_blocks->nb_index-1);
	
	for(i=0;i<parent_inode->dir_blocks->nb_index;i++){
		if(parent_inode->dir_blocks->tab_index[i].inode != inode_to_remove) {
			new_index[i] = parent_inode->dir_blocks->tab_index[i]; //copy the old index in the new one without the inode to remove
		}
	}
	
	free(parent_inode->dir_blocks->tab_index);
	parent_inode->dir_blocks->tab_index = new_index;
	
	parent_inode->dir_blocks->nb_index--;
}


Inode* search_file_in_directory(char* file_name,Directory_block* directory) {
	int i;
	
	if(directory != NULL) {
		for(i=0;i<directory->nb_index;i++) {
			if(strcmp(directory->tab_index[i].name,file_name) == 0) {
				return directory->tab_index[i].inode;
			}
		}
	}
	return NULL;
}

Inode* search_parent_inode(Inode* inode,Disk* disk) {
	Inode* parent_inode = disk->inodes;
	Directory_block* directory;
	int i;
	
	while(parent_inode != NULL) {
		if(parent_inode->dir_blocks != NULL) {
			directory = parent_inode->dir_blocks;
			if(directory->nb_index > 2) {
				for(i=0;i<directory->nb_index;i++) {
					if(directory->tab_index[i].inode == inode) {
						return parent_inode;
					}
				}
			}
		}
		parent_inode = parent_inode->next_inode;
	}
	return NULL;
}
					


