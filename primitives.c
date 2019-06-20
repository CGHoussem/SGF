#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "struct_SGF.h"
#include "primitives.h"
#include "command_shell.h"
#include "save_load_SGF.h"
#include "constants.h"

void mymkdir(char* name,Disk* disk,Inode* current_inode){
	Inode* inode = NULL;
	
	inode = (Inode*)malloc(sizeof(Inode));
	
	strcpy(inode->name, name);
	
	init_permissions(inode->permissions);
	inode->type=DIRECTORY; 
	inode->date_creation=time(NULL);
	inode->date_modification=time(NULL);
	
	inode->data_blocks = NULL;
	inode->dir_blocks = allocation_block_directory();
	
	inode->nb_data_blocks = 0;
	inode->nb_links = 1;

	inode->next_inode = NULL;
	
	if(current_inode == NULL){ //root
		init_block_directory(inode->dir_blocks,inode,inode,disk);
	}
	else{
		init_block_directory(inode->dir_blocks,inode,current_inode,disk);
		update_tab_index(current_inode,inode);
	}
	
	add_inode(inode,disk);
	
}

void mycreate(char* name,Disk* disk,Inode* current_inode){
    Inode* inode = NULL;
	
	inode = (Inode*)malloc(sizeof(Inode));
	
	strcpy(inode->name, name);
	
	init_permissions(inode->permissions);
	inode->type=TEXT; 
	inode->date_creation=time(NULL);
	inode->date_modification=time(NULL);
	
	inode->data_blocks = allocation_tab_block_data(1);

	inode->nb_data_blocks = 1;
	inode->dir_blocks = NULL;
	
	inode->nb_links = 1;

	inode->next_inode = NULL;
	
	init_block_data(inode->data_blocks[inode->nb_data_blocks-1],disk);
	update_tab_index(current_inode,inode);
		
	add_inode(inode,disk);
	
}

void myls(Inode* current_inode,char* name_index) {
	char file_type[MAX_FILE_NAME];
	char* file_name;
	
	
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
	printf("%s -> file type : %s, rights : %s\n",file_name, 
	file_type, current_inode->permissions);

}

void mycp(Inode** inodes,Inode* parent_dest,int number,Disk* disk){
	int i,j,k;
	Inode* source = NULL;
	Inode* dest = NULL;
	Inode* parent = NULL;
	
	for(i=0;i<number-1;i++) {
		source = inodes[i];
		
		if((inodes[number-1])->type == DIRECTORY){ //search destination
			dest = search_file_in_directory(source->name,(inodes[number-1])->dir_blocks);
			if(dest == NULL) { //file doesn't exist
				mycreate(source->name,disk,inodes[number-1]);
				dest = get_last_inode(*disk);
				parent = inodes[number-1];
			}
			for(j=0;j<10;j++) {
				dest->permissions[j] = source->permissions[j];
			}
			
		} else {
			dest = inodes[number-1];
			parent = parent_dest;
		}
		
		//copy the inode's informations
		
		dest-> date_modification = time(NULL);
						
		for(j=0;j<dest->nb_data_blocks;j++) {	
			if(dest->data_blocks[j]->size != 0) {
				for(k=0;k<dest->data_blocks[j]->size;k++){ //delete the old data
					dest->data_blocks[j]->data[k] = 0;
				}
			}
		}
		
		if(source->nb_data_blocks > dest->nb_data_blocks) { // source file has more data blocks than the destination
			int* reallocation = realloc(dest->data_blocks, source->nb_data_blocks); 
			if(reallocation == NULL) { // the realloc hasn't worked
				printf("Error while creating the new file.\n");
				remove_tab_index(dest,parent,disk);
				free_inode(disk,dest);
				return;
			}
			int offset = source->nb_data_blocks - dest->nb_data_blocks;
			for(j=offset;j<source->nb_data_blocks;j++) {
				dest->data_blocks[j] = allocation_block_data();
				dest->data_blocks[j] = source->data_blocks[j];
			}
			dest->nb_data_blocks = source->nb_data_blocks;
		}
		
		for(j=0;j<dest->nb_data_blocks;j++) {
			for(k=0;k<source->data_blocks[j]->size;k++){ //write the new data
				dest->data_blocks[j]->data[k] = source->data_blocks[j]->data[k];
			}
		}
	}
}		

void mycd (Inode *inode,Inode **current_inode){
	*current_inode = inode;
}

void mymv(Inode** inodes,Inode** parent_inode,int number,Disk* disk){
    
    int i;
	
	mycp(inodes,parent_inode[number-1],number,disk);
	for(i=0;i<number-1;i++) {
		myrm(inodes[i],parent_inode[i],disk);
	}
}

void myrm(Inode* inode,Inode* parent_inode,Disk* disk){
	remove_tab_index(inode,parent_inode,disk);
	inode->nb_links--;
	if(inode->nb_links == 0) {
		free_inode(disk,inode);
	}
	
}

char* myread(Inode* inode, char* output) {
	int i;

	for(i=0;i<inode->nb_data_blocks;i++) {
		sprintf(output, "%s", inode->data_blocks[i]->data);
	}
	return output;
}

void mywrite(Inode* inode,char* output,Disk* disk) {
	
	int id_last_tab = inode->nb_data_blocks-1;
	int size = inode->data_blocks[id_last_tab]->size;
	int available = BUFFER_SIZE-size;
	
	if(available > strlen(output)) { // any other data block is needed
		sprintf(inode->data_blocks[id_last_tab]->data, "%s", output);
		inode->data_blocks[id_last_tab]->size = strlen(inode->data_blocks[id_last_tab]->data);
		int str_size = inode->data_blocks[id_last_tab]->size;
		inode->data_blocks[id_last_tab]->data[str_size] = '\0';
	}
	else { //we have to create another or many other data block(s)
		inode->date_modification = time(NULL);
		inode->nb_data_blocks++;
		int* reallocation = realloc(inode->data_blocks, inode->nb_data_blocks); 
		if(reallocation == NULL) { // the realloc hasn't worked
			printf("Error while giving a new data block.\n");
			inode->nb_data_blocks--;
			return;
		}
			
		int i, position_output=0;
		// Completing the last data block
		for(i=size;i<BUFFER_SIZE;i++) {
			inode->data_blocks[id_last_tab]->data[i] = output[position_output];
			position_output++;
		}
		
		int offset = strlen(output)-available; // number of chars remaining
		int nb_realloc = inode->nb_data_blocks+1;
		
		if(offset < BUFFER_SIZE) { // The new data block will be sufficient
			// Writing in the new data block
			int* reallocation = realloc(inode->data_blocks, nb_realloc); 
			if(reallocation == NULL) { // the realloc hasn't worked
				printf("Error while allocating the new data block.\n");
				return;
			}
			inode->nb_data_blocks++;
			id_last_tab++;
			for(i=0;i<offset;i++) {
				inode->data_blocks[id_last_tab]->data[i] = output[position_output];
				position_output++;
			}
			
		} else { // The rest of the output requires another data block
			int extra = (int)round(strlen(output)/BUFFER_SIZE);
			nb_realloc += extra;
			
			// Writing in the new data blocks
			int* reallocation = realloc(inode->data_blocks, nb_realloc); 
			if(reallocation == NULL) { // the realloc hasn't worked
				printf("Error while allocating the new data blocks.\n");
				return;
			}
			inode->nb_data_blocks = nb_realloc;
			while(id_last_tab < nb_realloc-1) {
				for(i=0;i<BUFFER_SIZE;i++) {
					inode->data_blocks[id_last_tab]->data[i] = output[position_output];
					position_output++;
				}
				id_last_tab++;
			}
			
			// Completing the last data block
			id_last_tab++;
			int remaining_length = strlen(output)-position_output;
			for(i=0;i<remaining_length;i++) {
				inode->data_blocks[id_last_tab]->data[i] = output[position_output];
				position_output++;
			}
		}
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
	int available = DISK_BYTES_LIMIT-(disk->nb_data_blocks*BUFFER_SIZE);
	
	printf("\n#### Informations about your disk : ####\n\n");
	printf("Number of used inodes : %d\n", disk->nb_inode);
	printf("Number of used data blocks : %d\n", disk->nb_data_blocks);
	printf("Number of used directory blocks : %d\n", disk->nb_dir_blocks);
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
					nb_index = inodes[nb_arg-1]->dir_blocks->nb_index;
					strcpy(inodes[nb_arg-1]->dir_blocks->tab_index[nb_index-1].name,name_link);
				}
				inodes[i]->nb_links++;
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
