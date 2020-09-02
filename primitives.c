#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "struct_SGF.h"
#include "primitives.h"
#include "command_shell.h"
#include "utility.h"
#include "constants.h"

void mymkdir(char* name, Disk* disk, Inode* current_inode){
	Inode* inode = (Inode*) malloc(sizeof(Inode));

	inode->uid = count_inodes(*disk);
	strcpy(inode->name, name);
	init_permissions(inode->permissions);
	inode->type=DIRECTORY; 
	inode->creation_date=time(NULL);
	inode->modification_date=time(NULL);

	inode->datablocks = NULL;
	inode->dirblock = allocation_block_directory();

	inode->next_inode = NULL;
	
	if(current_inode == NULL){ //root
		init_block_directory(inode->dirblock, inode, inode, disk);
	}
	else {
		init_block_directory(inode->dirblock,inode,current_inode,disk);
		update_tab_index(current_inode,inode);
	}
	
	add_inode(inode,disk);
}

void mycreate(char* name,Disk* disk,Inode* current_inode){
    Inode* inode = malloc(sizeof(Inode));
	
	// Deleting uneeded chars from the name
	// char* temp_temp = name;
	// char* temp_name = malloc(sizeof(char) * strlen(name));
	// int i = 0;
	// while (*name != '\0')
	// {
	// 	if (
	// 		(*name >= 32 && *name <= 90) || 
	// 		(*name >= 97 && *name <= 122) || 
	// 		*name == '.' || 
	// 		*name == '-' || 
	// 		*name == '*' || 
	// 		*name == '_'
	// 	)
	// 	{
	// 		*(temp_name+i) = *name;
	// 		i++;
	// 	}
	// 	name++;
	// }
	// printf(BOLDBLUE"true name: %s\n"RESET, temp_name);
	// free(temp_name);

	inode->uid = count_inodes(*disk);
	strcpy(inode->name, name);
	
	init_permissions(inode->permissions);
	inode->type=TEXT; 
	inode->creation_date=time(NULL);
	inode->modification_date=time(NULL);
	
	//inode->datablocks = allocation_tab_block_data(1);
	inode->datablocks = NULL;
	inode->dirblock = NULL;

	inode->next_inode = NULL;
	
	update_tab_index(current_inode,inode);	
	add_inode(inode,disk);
}

void myls(Inode* current_inode,char* name_index) {
	char file_type[MAX_FILE_NAME];
	char* file_name = NULL;
	
	if(name_index == NULL) {
		file_name = current_inode->name;
	} else {
		file_name = name_index;
	}
	
	switch(current_inode->type) { //file type
		case TEXT:
			strcpy(file_type,"Text");
			break;
		case BINARY:
			strcpy(file_type,"Binary");
			break;
		case DIRECTORY:
			strcpy(file_type,"Directory");
			break;
		default:
			strcpy(file_type," ");
			break;
	}
	printf("%s -> file type: %s, rights: %s\n", file_name, file_type, current_inode->permissions);
}

void mycd (Inode *inode,Inode **current_inode){
	*current_inode = inode;
}

// void mycp2(Inode* file, Inode* dest_parent, Disk* disk)
// {
// 	Inode* destination = search_file_in_directory(file->name, );
// }

// void mycp(Inode** inodes,Inode* parent_dest,int number,Disk* disk){
// 	int i,j,k;
// 	Inode* source = NULL;
// 	Inode* dest = NULL;
// 	Inode* parent = NULL;

// 	for(i=0;i<number-1;i++) {
// 		source = inodes[i];
		
// 		if((inodes[number-1])->type == DIRECTORY){ //search destination
// 			dest = search_file_in_directory(source->name,(inodes[number-1])->dir_blocks);
// 			if(dest == NULL) { //file doesn't exist
// 				mycreate(source->name,disk,inodes[number-1]);
// 				dest = get_last_inode(*disk);
// 				parent = inodes[number-1];
// 			}
// 			for(j=0;j<10;j++) {
// 				dest->permissions[j] = source->permissions[j];
// 			}
			
// 		} else {
// 			dest = inodes[number-1];
// 			parent = parent_dest;
// 		}
		
// 		//copy the inode's informations
// 		dest-> date_modification = time(NULL);
		
// 		dest->data_blocks = allocation_block_data();
// 		dest->data_blocks->prev_block = NULL;
// 		// for(j=0;j<dest->nb_data_blocks;j++) {	
// 		// 	if(dest->data_blocks[j]->size != 0) {
// 		// 		for(k=0;k<dest->data_blocks[j]->size;k++){ //delete the old data
// 		// 			dest->data_blocks[j]->data[k] = 0;
// 		// 		}
// 		// 	}
// 		// }
		
// 		if(source->nb_data_blocks > dest->nb_data_blocks) { // source file has more data blocks than the destination
// 			int* reallocation = realloc(dest->data_blocks, source->nb_data_blocks); 
// 			if(reallocation == NULL) { // the realloc hasn't worked
// 				printf("Error while creating the new file.\n");
// 				remove_tab_index(dest,parent,disk);
// 				free_inode(disk,dest);
// 				return;
// 			}
// 			int offset = source->nb_data_blocks - dest->nb_data_blocks;
// 			for(j=offset;j<source->nb_data_blocks;j++) {
// 				dest->data_blocks[j] = allocation_block_data();
// 				dest->data_blocks[j] = source->data_blocks[j];
// 			}
// 			dest->nb_data_blocks = source->nb_data_blocks;
// 		}
		
// 		Data_block* tempDest = dest->data_blocks;
// 		Data_block* tempSrc = source->data_blocks; 
// 		while (tempDest != NULL && tempSrc != NULL)
// 		{
// 			strcpy(tempDest->data, tempDest->data);
// 			tempSrc = tempSrc->next_block;
// 			tempDest = tempDest->next_block;
// 		}
// 		// for(j=0;j<dest->nb_data_blocks;j++) {
// 		// 	for(k=0;k<source->data_blocks[j]->size;k++){ //write the new data
// 		// 		dest->data_blocks[j]->data[k] = source->data_blocks[j]->data[k];
// 		// 	}
// 		// }
// 	}
// }

void mymv(Inode** inodes,Inode** parent_inode,int number,Disk* disk){

	printf(BOLDBLUE"Moving ...\n"RESET);
	for (int i = 0; i < number+1; i++)
		printf(BOLDBLUE"Inode: %s\nparent_inode: %s\n"RESET, inodes[i]->name, parent_inode[number]->name);

	//mycp(inodes,parent_inode[number],number,disk);
	// for(int i=0;i<number;i++) {
	// 	myrm(inodes[i],parent_inode[i],disk);
	// }
}

void myrm(Inode* inode,Inode* parent_inode,Disk* disk){
	remove_tab_index(inode,parent_inode,disk);	
}

char* myread(Inode* inode) {
	char* output = malloc(BUFFER_SIZE*count_inode_datablocks(inode)*sizeof(char));
	output[0] = '\0';

	DataBlock* temp = inode->datablocks;
	while (temp != NULL)
	{
		strcat(output, temp->data);
		temp = temp->next_block;
	}
	return output;
}

void mywrite(Inode* inode,char* output,Disk* disk) {
	if (inode->datablocks == NULL){
		inode->datablocks = init_datablock();
	}
	DataBlock* db = inode->datablocks;
	if (strlen(output) > BUFFER_SIZE) {
		int blocks_to_allocate = strlen(output) / BUFFER_SIZE;
		print_debug("There is %d blocks to allocate for '%s'", blocks_to_allocate, output);

		if (strlen(output) % BUFFER_SIZE > 1)
			blocks_to_allocate++;
	
		for (int i = 0; i < blocks_to_allocate-1; i++)
		{
			char* string = substring(output, -1 + i*BUFFER_SIZE, BUFFER_SIZE);
			print_debug("saving datablock[%d].data = %s", i, string);
			sprintf(db->data, "%s", string);
			free(string);
			if (db->next_block == NULL){
				db->next_block = allocation_block_data();
				db->next_block->prev_block = db;
			}
			db = db->next_block;
		}

		char* string = substring(output, -1 + BUFFER_SIZE*(blocks_to_allocate-1), strlen(output)-BUFFER_SIZE*(blocks_to_allocate-1));
		print_debug("last datablock.data = %s", string);
		strcpy(db->data, string);
		free(string);
	} else {
		strcpy(db->data, output);
	}
}


void mychmod(Inode** inodes,int number,char permissions[9],Disk* disk){
	int i;
	for(i=0;i<number;i++) {	
		if(inodes[i] != NULL) { //file does exist
			inodes[i]->permissions[0] = permissions[0];
			inodes[i]->permissions[1] = permissions[1];
			inodes[i]->permissions[2] = permissions[2];
			inodes[i]->permissions[3] = permissions[3];
			inodes[i]->permissions[4] = permissions[4];
			inodes[i]->permissions[5] = permissions[5];
			inodes[i]->permissions[6] = permissions[6];
			inodes[i]->permissions[7] = permissions[7];
			inodes[i]->permissions[8] = permissions[8];
		}
		else { //file doesn't exist
			printf("The file or directory %s doesn't exist here ! \n", inodes[i]->name);
		}
	}		
}

void mydf(Disk* disk) {
	unsigned int nb_datablocks = count_disk_datablocks(*disk);
	int available = DISK_BYTES_LIMIT-(nb_datablocks*BUFFER_SIZE);
	
	printf("\n#### Informations about your disk : ####\n\n");
	printf("Number of used inodes : %d\n", count_inodes(*disk));
	printf("Number of used data blocks : %d\n", nb_datablocks);
	printf("Number of used directory blocks : %d\n", count_disk_dirblocks(*disk));
	printf("Available space on the disk :  %d bytes\n\n", available);
}

void myln(Inode** inodes,Inode* current_inode,int nb_arg, Disk* disk,char name_link[MAX_FILE_NAME]){
	int i,nb_index;
	
	if (nb_arg == 1 && inodes[0] != NULL){ //add in the current directory an index associated with this inode without changing the name of the file.
		update_tab_index(current_inode,inodes[0]);
	} else if (nb_arg == 1 && inodes[0] == NULL) {
		printf("Error: argument 1 doesn't exist \n");
	} else if (inodes[nb_arg-1] != NULL && inodes[nb_arg-1]->type == DIRECTORY) {
		for(i=0;i<nb_arg-1;i++) {
			if(inodes[i] == NULL) {
				printf("Error : argument %d doesn't exist \n",i+1);
			} else if(inodes[i]->type == DIRECTORY) {
				printf("Error : argument %d is a directory \n",i+1);
			} else {
				update_tab_index(inodes[nb_arg-1],inodes[i]);
				if(strcmp(name_link,"\0") != 0) {
					nb_index = inodes[nb_arg-1]->dirblock->nb_index;
					strcpy(inodes[nb_arg-1]->dirblock->tab_index[nb_index-1].name,name_link);
				}
				// TODO to look into the number of links
				// inodes[i]->nb_links++;
			}
		}
	} else if (nb_arg == 2 && inodes[nb_arg-1] != NULL && inodes[nb_arg-1]->type != DIRECTORY) {
		printf("Impossible to link, the argument 2 already exist \n");
	} 
	/*else if(nbr_inode==2){ //create a symbolic link (once again, an index) of the first inode given, and will give the name of the second one to the created index.
		update_tab_index(current_inode,inode[0]);
		Inode* tmp=inode[0];
		inode[1]=tmp;
		
		update_tab_index(current_inode,inode[1]);
	}
	else{ //add to the directory block the index of each file in the array (except the directory) and without changing the name.
		//inode->dir_blocks->tab_index[i]
		for (int i=0;i<nbr_inode;i++){
		
			update_tab_index(current_inode, inode[i]);
		}
	}*/
}
