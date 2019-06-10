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

/*
cat : pour concaténer des fichiers (peut display mais aussi rediriger)
ln : pour créer un lien symbolique entre deux fichiers
echo “texte” > file : pour écrire dans un fichier
*/
//TODO vérifier que le disque ne soit pas complet avant d'ajouter

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
	
	//inode->nb_data_blocks = 1;
	inode->data_blocks = allocation_tab_block_data(1);

	inode->nb_data_blocks = 1;
	inode->dir_blocks = NULL;

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

void mycp(Inode** inodes,int number,Disk* disk){
	int i,j,k;
	Inode* source = NULL;
	Inode* dest = NULL;
	
	for(i=0;i<number-1;i++) {
		source = inodes[i];
		
		if((inodes[number-1])->type == DIRECTORY){ //search destination
			dest = search_file_in_directory(source->name,(inodes[number-1])->dir_blocks);
			if(dest == NULL) { //file doesn't exist
				mycreate(source->name,disk,inodes[number-1]);
				dest = get_last_inode(*disk);
			}
		} else {
			dest = inodes[number-1];
		}

		//copy the inode's informations
		for(j=0;j<9;j++){
			dest->permissions[i] = source->permissions[i];
		}
		dest-> date_modification = time(NULL);
				
		for(j=0;i<dest->nb_data_blocks;i++) {	
			if(dest->data_blocks[i]->size != 0) {
				for(k=0;k<dest->data_blocks[j]->size;k++){ //delete the old data
					dest->data_blocks[j]->data[k] = 0;
				}
			}
		}
		
		if(source->nb_data_blocks > 1) { // source file has more data blocks than the destination
			dest->nb_data_blocks = source->nb_data_blocks;
			int* reallocation = realloc(dest->data_blocks, dest->nb_data_blocks); 
			if(reallocation == NULL) { // the realloc hasn't worked
				printf("Error while creating the new file.\n");
				remove_tab_index(dest,disk);
				free_inode(disk,dest);
				return;
			}
			for(j=0;i<dest->nb_data_blocks;i++) {
				dest->data_blocks[j] = source->data_blocks[j];
			}
		}
		
		for(j=0;i<dest->nb_data_blocks;i++) {
			dest->data_blocks[i]->size = source->data_blocks[i]->size;
			strcpy(dest->data_blocks[i]->data, source->data_blocks[i]->data);
			/*for(j=0;j<dest->data_blocks[j]->size;j++) { //write the new data
				dest->data_blocks[i]->data = source->data_blocks[i]->data[j];
			}*/
		}
	}		
}

void mycd (Inode *inode,Inode **current_inode){
	*current_inode = inode;
}

void mymv(Inode** inodes,int number,Disk* disk){
    
    int i;
	//Inode* source = NULL;
	//Inode* dest = NULL;
	
	mycp(inodes,number,disk);
	for(i=0;i<number-1;i++) {
		myrm(inodes[i],disk);
	}
}

void myrm(Inode* inode,Disk* disk){
	remove_tab_index(inode,disk);
	free_inode(disk,inode);
	
}

char* myread(Inode* inode, char* output) {
	int i;
	printf("data : %s\n", inode->data_blocks[0]->data);
	for(i=0;i<inode->nb_data_blocks;i++) {
		strcat(output, inode->data_blocks[i]->data);
	}
	return output;
}

void mywrite(Inode* inode,char* output,Disk* disk) {
	
	int id_last_tab = inode->nb_data_blocks-1;
	int size = inode->data_blocks[id_last_tab]->size;
	int available = BUFFER_SIZE-size;
	
	if(available > strlen(output)) { // any other data block is needed
		strcat(inode->data_blocks[id_last_tab]->data, output);
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

void myrmdir(Inode** inodes,int number,Disk* disk){
	
	int i;
	printf("%d repertoires à supprimer\n", number);
	for(i=0;i<number;i++) {
		printf("%s\n", inodes[i]->name);
		if(inodes[i] != NULL) { //file does exist
			remove_tab_index(inodes[i],disk);
			free_inode(disk,inodes[i]);
		}
		else { //file doesn't exist
			printf("The directory %s doesn't exist here ! \n", inodes[i]->name);
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
	int available = DISK_BYTES_LIMIT-(disk->nb_data_blocks*1024);
	
	printf("\n#### Informations about your disk : ####\n\n");
	printf("Nombre d'inodes utilisés : %d\n", disk->nb_inode);
	printf("Nombre de blocs de données utilisés : %d\n", disk->nb_data_blocks);
	printf("Nombre de blocs de répertoires utilisés : %d\n", disk->nb_dir_blocks);
	printf("Taille de l'espace disponible :  %d octets\n\n", available);
}

void myln(Inode** inodes,Inode* current_inode,int nb_arg, Disk* disk){
	int i;
	
	if (nb_arg == 1){ //add in the current directory an index associated with this inode without changing the name of the file.
		update_tab_index(current_inode,inodes[0]);
	} else if (inodes[nb_arg-1] != NULL && inodes[nb_arg-1]->type == DIRECTORY) {
		for(i=0;i<nb_arg-1;i++) {
			if(inodes[i] == NULL) {
				printf("Error : argument %d doesn't exist \n",i+1);
			} else if(inodes[i]->type == DIRECTORY) {
				printf("Error : argument %d is a directory \n",i+1);
			} else {
				update_tab_index(inodes[nb_arg-1],inodes[i]);
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
