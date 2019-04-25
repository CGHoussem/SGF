#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "struct_SGF.h"
#include "primitive.h"
#include "command_shell.h"
#include "save_load_SGF.h"

void format_disk(Disk* disk){
	Inode inode_null, inode;
	char name[]="root";
	
	inode=mkdir(name,inode_null);
	disk->inodes=&inode;
	disk->nb_inode=1;
	disk->blocks=disk->inodes->tab_block;
	disk->nb_block=1;
	
	disk->blocks->b_directory->tab_index[1].inode=&inode;//change the inode of the directory ".."
	
}

Inode mkdir(char* name,Inode prev_inode){
	Inode inode;
	
	strcpy(inode.name,name);
	init_permissions(inode.permissions);
	inode.type=3; //directory
	inode.date_creation=time(NULL);
	inode.date_modification=time(NULL);
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
