#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "struct_SGF.h"
#include "primitives.h"
#include "command_shell.h"
#include "save_load_SGF.h"
#include "constants.h"

Inode mkdir(char* name,Inode* prev_inode){
	Inode inode;
	
	strcpy(inode.name,name);
	init_permissions(inode.permissions);
	inode.type=DIRECTORY; 
	inode.date_creation=time(NULL);
	inode.date_modification=time(NULL);
	
	inode.data_blocks = NULL;
	inode.dir_blocks = allocation_tab_block_directory(1);

	init_block_directory(inode.dir_blocks, &inode, prev_inode);
	
	inode.prev_inode=prev_inode;
	inode.next_inode=NULL;
	
	
	return inode;
}
