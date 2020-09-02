#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "struct_SGF.h"
#include "primitives.h"
#include "command_shell.h"
#include "utility.h"
#include "constants.h"

#if DEBUG == 1

void dump_disk_inodes(Disk* disk) {
	Inode* head = disk->inodes;
	printf(HIGHTLIGHT"\n==========\n");
	printf("DUMPING DISK\n");
	printf("==========\n");
	while (head != NULL) {
		printf("Inode %d\n", head->uid);
		head = head->next_inode;
	}
	printf("=========="RESET"\n");
}

void dump_disk_indexes(Disk* disk) {
	Inode* head = disk->inodes;
	printf(HIGHTLIGHT"\n==========\n");
	printf("DUMPING DISK\n");
	printf("==========\n");
	while (head != NULL) {
		
		printf("Inode %d\n", head->uid);

		if (head->type == 3) {
			for (int i = 0; head->dirblock != NULL && i < head->dirblock->nb_index; i++) {
				printf("\tIndex: %s - %d\n", head->dirblock->tab_index[i].name, head->dirblock->tab_index[i].inode->uid);
			}
		} else {
			printf(RED"\tNOT DIRECTORY!\n"HIGHTLIGHT);
		}

		head = head->next_inode;
	}
	printf("=========="RESET"\n");
}

#endif

unsigned int count_inodes(Disk disk)
{
	unsigned int size = 0;
	Inode* temp = disk.inodes;
	while (temp)
	{
		size++;
		temp = temp->next_inode;
	}
	return size;
}

unsigned int count_inode_datablocks(Inode *inode)
{
	unsigned int size = 0;
	DataBlock* temp = inode->datablocks;
	while (temp)
	{
		size++;
		temp = temp->next_block;
	}
	return size;
}

unsigned int count_inode_dirblocks(Inode inode)
{
	unsigned int size = 0;
	DirectoryBlock* temp = inode.dirblock;
	while (temp)
	{
		size++;
		temp = temp->next_block;
	}
	return size;
}

unsigned int count_disk_datablocks(Disk disk)
{
	unsigned int size = 0;
	Inode* temp = disk.inodes;
	while (temp)
	{
		size += count_inode_datablocks(temp);
		temp = temp->next_inode;
	}
	return size;
}

unsigned int count_disk_dirblocks(Disk disk)
{
	unsigned int size = 0;
	Inode* temp = disk.inodes;
	while (temp)
	{
		size += count_inode_dirblocks(*temp);
		temp = temp->next_inode;
	}
	return size;
}

void format_disk(Disk* disk){
	disk->inodes = NULL;
	mymkdir("root",disk,NULL);

	printf(BOLDGREEN"The disk has been successfully formatted!\n"RESET);
}

DirectoryBlock* allocation_block_directory(){
	DirectoryBlock* tab_block = NULL;
	
	tab_block= (DirectoryBlock*) malloc(sizeof(DirectoryBlock));
	tab_block->prev_block = NULL;
	tab_block->next_block = NULL;
	
	return tab_block;
}

DataBlock* allocation_block_data(){
	DataBlock* block = NULL;
	
	block = malloc(sizeof(DataBlock));
	block->next_block = NULL;
	block->prev_block = NULL;
		
	return block;
}

DataBlock* init_datablock() {
	DataBlock *block = malloc(sizeof(DataBlock));
	block->next_block = NULL;
	block->prev_block = NULL;
	return block;
}

void init_block_directory(DirectoryBlock* block, Inode* inode_directory, Inode* inode_parent_directory, Disk* disk){
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
	//add_dir_block(block, disk);
}

Index* allocation_index(int size){
	Index* index = malloc(size*sizeof(Index));
	
	return index;
}

void init_permissions(char permissions[10]){
	permissions[0]='r';
	permissions[1]='w';
	permissions[2]='x';
	permissions[3]='r';
	permissions[4]='w';
	permissions[5]='-';
	permissions[6]='r';
	permissions[7]='-';
	permissions[8]='-';
	permissions[9]='\0';
}

void delete_permissions(char permissions[10]){
	permissions[0]='-';
	permissions[1]='-';
	permissions[2]='-';
	permissions[3]='-';
	permissions[4]='-';
	permissions[5]='-';
	permissions[6]='-';
	permissions[7]='-';
	permissions[8]='-';
}

void free_inode(Inode* inode){
	if(inode->type == DIRECTORY){ 
		free_dirblock(inode->dirblock);
		inode->dirblock = NULL;
	} else {

		if (inode->datablocks == NULL)
			perror("NULL!");

		while (inode->datablocks != NULL)
		{
			DataBlock* temp = inode->datablocks;
			inode->datablocks = inode->datablocks->next_block;
			free(temp);
		}
		free(inode->datablocks);
		inode->datablocks = NULL;
	}
	free(inode);
}

void free_dirblock(DirectoryBlock* block){
	free(block->tab_index);
	free(block);
}

void free_datablock(DataBlock* block){
	free(block);
}

void free_disk(Disk* disk){
	while(disk->inodes != NULL){
		Inode* temp = disk->inodes;
		disk->inodes = disk->inodes->next_inode;
		free_inode(temp);
	}
	free(disk);
}

Inode* get_last_inode(Disk disk){
	Inode* current_inode = disk.inodes;
	
	while(current_inode != NULL && current_inode->next_inode != NULL){
		current_inode = current_inode->next_inode;
	}
	
	return current_inode;
}

void add_inode(Inode* inode, Disk* disk){
	if(disk->inodes != NULL){
		Inode* last_inode = get_last_inode(*disk);
		
		last_inode->next_inode = inode;
		//inode->prev_inode = last_inode;
	}
	else{
		disk->inodes = inode;
		//inode->prev_inode = NULL;
	}
	
}

DirectoryBlock* get_last_dir_block(Disk disk){
	DirectoryBlock* current_dir_block = disk.inodes[count_inodes(disk)-1].dirblock;
	
	while(current_dir_block != NULL && current_dir_block->next_block != NULL){
		current_dir_block = current_dir_block->next_block;
	}
	
	return current_dir_block;
}

// TODO //add_dir_block to look into
// void //add_dir_block(Directory_block* dir_block, Disk* disk){
// 	if(disk->dir_blocks != NULL){
// 		Directory_block* last_dir_block = get_last_dir_block(*disk);
		
// 		last_dir_block->next_block = dir_block;
// 		dir_block->prev_block = last_dir_block;
		
// 		disk->nb_dir_blocks++;
// 	}
// 	else{
// 		disk->dir_blocks = dir_block;
// 		dir_block->prev_block = NULL;
// 		disk->nb_dir_blocks = 1;
// 	}
// }
	
DataBlock* get_last_data_block(Disk disk){
	DataBlock* current_data_block = disk.inodes[count_inodes(disk)-1].datablocks;
	
	while(current_data_block != NULL && current_data_block->next_block != NULL){
		current_data_block = current_data_block->next_block;
	}
	
	return current_data_block;
}

DataBlock* get_last_inode_data_block(Inode* inode){
	DataBlock* temp = inode->datablocks;
	while (temp != NULL)
		temp = temp->next_block;
	return temp;
}

// Todo: ////add_data_block to look into
// void ////add_data_block(Data_block* data_block, Disk* disk){
// 	if(disk->data_blocks != NULL){
// 		Data_block* last_data_block = get_last_data_block(*disk);
		
// 		last_data_block->next_block = data_block;
// 		data_block->prev_block = last_data_block;
		
// 		disk->nb_data_blocks++;
// 	}
// 	else{
// 		disk->data_blocks = data_block;
// 		data_block->prev_block = NULL;
// 		disk->nb_data_blocks = 1;
// 	}
// }

void update_tab_index(Inode* current_inode, Inode* inode_to_add){
	Index* new_index = NULL;
	int i;
	
	/////// DEBUG ////////
	if (current_inode->dirblock == NULL){
		fprintf(stderr, BOLDRED"System error!\n"RESET);
		exit(EXIT_FAILURE);
	}
	//////////////////////

	new_index = allocation_index(current_inode->dirblock->nb_index+1);
	
	for(i=0;i<current_inode->dirblock->nb_index;i++){
		new_index[i] = current_inode->dirblock->tab_index[i]; //copy the old index in the new one
	}
	
	//add the directory at the end of the index
	strcpy(new_index[current_inode->dirblock->nb_index].name,inode_to_add->name);
	new_index[current_inode->dirblock->nb_index].inode = inode_to_add;
	
	free(current_inode->dirblock->tab_index);
	current_inode->dirblock->tab_index = new_index;
	
	current_inode->dirblock->nb_index++;
	
}

void remove_tab_index(Inode* inode_to_remove, Inode* parent_inode, Disk* disk){
	int inode_uid = inode_to_remove->uid;
	
	{ // delete the inode from the disk
		char deleted = 0;
		Inode* head = disk->inodes;
		if (inode_uid == head->uid) {
			// remove the inode from the disk!
			Inode* temp = head;
			head = head->next_inode;
			free(temp);
			deleted = 1;
		}	

		Inode* current = head->next_inode;
		Inode* previous = head;
		while (deleted == 0 && current != NULL && previous != NULL) {
			if (inode_uid == current->uid) {
				// remove the inode from the disk!
				Inode* temp = current;
				previous->next_inode = current->next_inode;
				free(temp);
				deleted = 1;
			}
			previous = current;
			current = current->next_inode;
		}
	}

	// TODO: TO REWORK !! (PERFORMANCE WASTE)
	// waste of memory and performance, we shouldn't copy tab_index to be able to delete one index!
	// todo: change tab_indexe from static table to a dynamic list!

	{ // delete the index from the dirblocks of the parent inode
		Index* new_index = NULL;
		int i;
		int j = 0;
		char delete = 0;

		new_index = allocation_index(parent_inode->dirblock->nb_index-1);
		
		for(i=0;i<parent_inode->dirblock->nb_index;i++){
			if(delete == 1 || parent_inode->dirblock->tab_index[i].inode->uid != inode_uid) {
				new_index[j] = parent_inode->dirblock->tab_index[i]; //copy the old index in the new one without the inode to remove
				j++;
			} else {
				delete = 1;
			}
		}
		
		free(parent_inode->dirblock->tab_index);
		parent_inode->dirblock->tab_index = new_index;
		
		parent_inode->dirblock->nb_index--;
	}

	#if DEBUG == 1
	dump_disk_indexes(disk);
	#endif
}

Inode* search_file_in_directory(char* file_name,DirectoryBlock* directory) {
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
	DirectoryBlock* directory;
	int i;
	
	while(parent_inode != NULL) {
		if(parent_inode->dirblock != NULL) {
			directory = parent_inode->dirblock;
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
					
char* substring(char* string, int position, int length){
	char *pointer;
	int c;

	pointer = malloc(length+1);

	if (pointer == NULL)
	{
		printf(BOLDRED"ERROR: Unable to allocate memory.\n"RESET);
		exit(1);
	}

	for (c = 0; c < length; c++)
	{
		*(pointer+c) = *(string+position+1);

		string++;
	}

	*(pointer+c) = '\0';
	return pointer;
}

