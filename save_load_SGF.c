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
	
	inode=mkdir(name,inode_null);
	disk->inodes=&inode;
	disk->nb_inode=1;
	disk->blocks=disk->inodes->tab_block;
	disk->nb_block=1;
	
	disk->blocks->b_directory->tab_index[1].inode=&inode;//change the inode of the directory ".."
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

Inode mkdir(char* name,Inode prev_inode){
	Inode inode;
	
	strcpy(inode.name,name);
	init_permissions(inode.permissions);
	inode.type=3; //directory
	inode.date_creation=time(NULL);
	inode.date_modification=time(NULL);
	
	inode.tab_block = (Block*) malloc(sizeof(Block));
	inode.tab_block->b_directory=allocation_tab_block_directory(1);

	init_block_directory(inode.tab_block->b_directory,inode,prev_inode);
	inode.next_inode=NULL;
	
	return inode;
}

Block_directory* allocation_tab_block_directory(int size){
	Block_directory* tab_block=NULL;
	
	tab_block=(Block_directory*)malloc(size*sizeof(Block_directory));
	
	return tab_block;
}

void init_block_directory(Block_directory* block,Inode inode,Inode prev_inode){
	char name_dir[]=".";
	char name_dir_father[]="..";
	
	block->tab_index=allocation_index(2);
	
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
	
	if(inode->type==3){ //directory
		free_block_directory(disk,inode->tab_block);
	}
	else{
		free_block_data(disk,inode->tab_block);
	}
	
	free(inode);
	
	disk->nb_inode--;
}

void free_block_directory(Disk* disk, Block* block){
	//Block* prev_block=search_prev_block(disk->blocks,block);
	
	//Comment gérer le fait qu'on connaisse pas la nature du bloc précédent ? --Solenn
	
	//prev_block->next_block=block->next_block;
	
	free(block->b_directory->tab_index);
	free(block->b_directory);
	free(block);
	
	disk->nb_block--;
	
}

void free_block_data(Disk* disk, Block* block){
	//Block* prev_block=search_prev_block(disk->blocks,block);
	
	//idem --Solenn
	//prev_block->next_block=block->next_block;
	
	free(block->b_data);
	free(block);
	
	disk->nb_block--;
	
}

/*
Block* search_prev_block(Block* first_block,Block* block){
	
	idem --Solenn
	
	if(first_bloc->next_block==block){
		return first_block;
	}
	else{
		return search_prev_block(first_block->next_block,block);
	}
	
}*/
	
