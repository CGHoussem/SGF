#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "struct_SGF.h"
#include "primitives.h"
#include "command_shell.h"
#include "utility.h"
#include "constants.h"


unsigned int count_indexes(DirectoryBlock* block) {
	unsigned int size = 0;
	Index* current = block->indexes;
	while (current != NULL) {
		size++;
		current = current->next_index;
	}
	return size;
}

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
	mymkdir("root", disk, NULL);

	printf(BOLDGREEN"The disk has been successfully formatted!\n"RESET);
}

DirectoryBlock* allocate_directory_block(){
	DirectoryBlock* tab_block = malloc(sizeof(DirectoryBlock));
	tab_block->next_block = NULL;
	
	return tab_block;
}

DataBlock* allocate_datablock(){
	DataBlock* block = NULL;
	
	block = malloc(sizeof(DataBlock));
	block->next_block = NULL;
		
	return block;
}

DataBlock* init_datablock() {
	DataBlock *block = malloc(sizeof(DataBlock));
	block->next_block = NULL;
	return block;
}

void init_directory_block(DirectoryBlock* block, Inode* inode_directory, Inode* inode_parent_directory, Disk* disk){
	char name_parent_dir[] = "..";
	char name_dir[] = ".";
	
	block->indexes = NULL;

	Index* index = malloc(sizeof(Index));
	strcpy(index->name, name_dir);
	index->inode = inode_directory;
	index->next_index = NULL;
	block->indexes = append_index_to_list(block->indexes, index);

	Index* index2 = malloc(sizeof(Index));
	strcpy(index2->name, name_parent_dir);
	index2->inode = inode_parent_directory;
	index2->next_index = NULL;
	block->indexes = append_index_to_list(block->indexes, index2);
	
	block->next_block = NULL;
	//add_dir_block(block, disk);
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
		while (inode->datablocks != NULL)
		{
			DataBlock* temp = inode->datablocks;
			inode->datablocks = inode->datablocks->next_block;
			free(temp);
			temp = NULL;
		}
		free(inode->datablocks);
		inode->datablocks = NULL;
	}
	free(inode);
}

void free_index(Index* index) {
	while (index != NULL) {
		Index* temp = index;
		index = index->next_index;
		free(temp);
	}
	free(index);
	index = NULL;
}

void free_dirblock(DirectoryBlock* block){
	free_index(block->indexes);

	free(block);
	block = NULL;
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
	}
	else{
		disk->inodes = inode;
	}
	
}

DirectoryBlock* get_last_dir_block(Disk disk){
	DirectoryBlock* current_dir_block = disk.inodes[count_inodes(disk)-1].dirblock;
	
	while(current_dir_block != NULL && current_dir_block->next_block != NULL){
		current_dir_block = current_dir_block->next_block;
	}
	
	return current_dir_block;
}

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

Index* append_index_to_list(Index* list, Index* index){
    if (list == NULL){
        list = index;
    } else if (index != NULL) {
        Index* temp = list;

        while (temp->next_index != NULL){
            temp = temp->next_index;
        }

        temp->next_index = index;
    }
    return list;
}

DataBlock* append_datablock_to_list(DataBlock* list, DataBlock* block){
    if (list == NULL){
        list = block;
    } else if (block != NULL) {
        DataBlock* temp = list;

        while (temp->next_block != NULL){
            temp = temp->next_block;
        }

        temp->next_block = block;
    }
    return list;
}

DataBlock* copy_datablock_list(DataBlock* source_list) {
	if (source_list == NULL)
		return NULL;

	DataBlock* head = malloc(sizeof(DataBlock));
	strcpy(head->data, source_list->data);

	DataBlock* p = head;
	source_list = source_list->next_block;
	while (source_list != NULL) {
		p->next_block = malloc(sizeof(DataBlock));
		p = p->next_block;
		strcpy(p->data, source_list->data);
		source_list = source_list->next_block;
	}

	p->next_block = NULL;

	return head;
}

void clear_datablocks(DataBlock* list) {
	while (list != NULL) {
		DataBlock* temp = list;
		list = list->next_block;
		free(temp);
	}
	free(list);
	list = NULL;
}

void update_tab_index(Inode* current_inode, Inode* inode_to_add){	
	if (current_inode->dirblock == NULL){
		print_debug("system error!");
		exit(EXIT_FAILURE);
	}

	Index* new_index = malloc(sizeof(Index));
	strcpy(new_index->name, inode_to_add->name);
	new_index->inode = inode_to_add;
	new_index->next_index = NULL;
	current_inode->dirblock->indexes = append_index_to_list(current_inode->dirblock->indexes, new_index);	
}

void delete_inode(Inode** head_ref, unsigned long inode_uid) {
	Inode* temp = *head_ref;
	Inode* prev = NULL;

	if (temp != NULL && temp->uid == inode_uid) {
		*head_ref = temp->next_inode;
		free_inode(temp);
		return;
	}

	while (temp != NULL && temp->uid != inode_uid) {
		prev = temp;
		temp = temp->next_inode;
	}

	if (temp == NULL) return;

	prev->next_inode = temp->next_inode;

	free_inode(temp);
}

void delete_index(Index** head_ref, const char* name) {
	Index* temp = *head_ref;
	Index* prev = NULL;

	if (temp != NULL && strcmp(temp->name, name) == 0) {
		*head_ref = temp->next_index;
		free(temp);
		return;
	}

	while (temp != NULL && strcmp(temp->name, name) != 0) {
		prev = temp;
		temp = temp->next_index;
	}

	if (temp == NULL) return;

	prev->next_index = temp->next_index;

	free(temp);
}

void remove_tab_index(Inode* inode_to_remove, Inode* parent_inode, Disk* disk){
	unsigned long inode_uid = inode_to_remove->uid;
	char* index_name = NULL;
	bool copied = false;

	print_debug("to remove inode id: %ld", inode_uid);
	
	Index* current = parent_inode->dirblock->indexes;
	while (copied == false && current != NULL) {
		if (current->inode->uid == inode_uid){
			index_name = malloc(sizeof(char) * strlen(current->inode->name)+1);
			strcpy(index_name, current->inode->name);
			copied = true;
		}
		current = current->next_index;
	}

	delete_inode(&disk->inodes, inode_uid);
	delete_index(&parent_inode->dirblock->indexes, index_name);

	free(index_name);
}

Inode* search_file_in_directory(char* file_name,DirectoryBlock* directory) {
	Inode* file = NULL;
	if(directory != NULL) {
		Index* current = directory->indexes;
		while (file == NULL && current != NULL) {
			if(strcmp(current->name,file_name) == 0) {
				file = current->inode;
			}
			current = current->next_index;
		}
	}
	return file;
}

Inode* search_parent_inode(Inode* inode,Disk* disk) {
	Inode* parent_inode = disk->inodes;
	Inode* ret = NULL;
	
	while(parent_inode != NULL) {
		if(parent_inode->dirblock != NULL) {
			DirectoryBlock* directory = parent_inode->dirblock;
			Index* current = directory->indexes;
			while (ret == NULL && current != NULL) {
				if (current->inode->uid == inode->uid) {
					ret = parent_inode;
				}
				current = current->next_index;
			}
		}
		parent_inode = parent_inode->next_inode;
	}
	return ret;
}
					
char* substring(char* string, int position, int length){
	char *pointer;
	int c;

	pointer = malloc(length+1);

	for (c = 0; c < length; c++)
	{
		*(pointer+c) = *(string+position+1);

		string++;
	}

	*(pointer+c) = '\0';
	return pointer;
}

