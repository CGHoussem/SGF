#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "struct_SGF.h"
#include "primitives.h"
#include "command_shell.h"
#include "save_load_SGF.h"
#include "constants.h"

void mkdir(char* name,Disk* disk,Inode* current_inode){
	Inode* inode = NULL;
	
	inode = (Inode*)malloc(sizeof(Inode));
	
	strcpy(inode->name, name);
	
	init_permissions(inode->permissions);
	inode->type=DIRECTORY; 
	inode->date_creation=time(NULL);
	inode->date_modification=time(NULL);
	
	inode->data_blocks = NULL;
	inode->dir_blocks = NULL;
	inode->dir_blocks = allocation_tab_block_directory(1);

	inode->next_inode = NULL;
	
	if(current_inode == NULL){ //root
		init_block_directory(inode->dir_blocks,inode,inode,disk);
	}
	else{
		init_block_directory(inode->dir_blocks,inode,current_inode,disk);
		update_index(current_inode,inode);
	}
	

	add_inode(inode,disk);

	
	printf("The directory %s has been created successfully ! \n",name);
	
}
