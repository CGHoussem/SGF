#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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

void mkdir(char* name,Disk* disk,Inode* current_inode){
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

void ls(Inode* current_inode) {
	int number = current_inode->dir_blocks->nb_index;
	char file_type[MAX_FILE_NAME];
	
	for(int i=0;i<number;i++) {
		switch(current_inode->dir_blocks->tab_index[i].inode->type) { //file type
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
		printf("%s -> file type : %s, rights : %s\n",current_inode->dir_blocks->tab_index[i].name, 
		file_type, current_inode->dir_blocks->tab_index[i].inode->permissions);
	}
}

void cp(Inode** inodes,int number,Disk* disk){
	int i,j;
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
				
		for(i=0;i<dest->nb_data_blocks;i++) {	
			if(dest->data_blocks[i]->size != 0) {
				for(j=0;j<dest->data_blocks[i]->size;j++){ //delete the old data
					dest->data_blocks[i]->data[j] = 0;
				}
			}
		}
		
		if(source->nb_data_blocks > 1) { // source file has more data blocks than the destination
			dest->nb_data_blocks = source->nb_data_blocks;
			int* reallocation = realloc(dest->data_blocks, dest->nb_data_blocks); 
			if (reallocation == NULL) { // the realloc hasn't work
				printf("Error while creating the new file.\n");
				remove_tab_index(dest,disk);
				free_inode(disk,dest);
				return;
			}
			for(i=0;i<dest->nb_data_blocks;i++) {
				dest->data_blocks[i] = source->data_blocks[i];
			}
		}
		
		for(i=0;i<dest->nb_data_blocks;i++) {
			dest->data_blocks[i]->size = source->data_blocks[i]->size;
			strcpy(dest->data_blocks[i]->data, source->data_blocks[i]->data);
			/*for(j=0;j<dest->data_blocks[j]->size;j++) { //write the new data
				dest->data_blocks[i]->data = source->data_blocks[i]->data[j];
			}*/
		}
	}		
}

void cd (char *name,Inode *current_inode, Disk* disk){	
	Directory_block* directory;
	directory= current_inode->dir_blocks;
	if (search_file_in_directory(name,directory))
	{
		current_inode = search_file_in_directory(name,directory);
		disk->inodes=current_inode;
	}
	else 
	{
		printf("This file %s doesn't exist\n",name);
	}
}

void mymv(Inode** inodes,int number,Disk* disk){
    
    int i;
	//Inode* source = NULL;
	//Inode* dest = NULL;
	
	cp(inodes,number,disk);
	for(i=0;i<number-1;i++) {
		myrm(inodes[i],disk);
	}
}

void myrm(Inode* inode,Disk* disk){
	remove_tab_index(inode,disk);
	free_inode(disk,inode);
	
}

void myread(Inode* inode) {
	printf("%s\n", inode->data_blocks[inode->nb_data_blocks-1]->data);
}

void mywrite(Inode* inode,char output[BUFFER_SIZE],Disk* disk) {
	
	int id_last_tab = inode->nb_data_blocks-1;
	int size = inode->data_blocks[id_last_tab]->size;
	int available = BUFFER_SIZE-size;
	
	if(available >= strlen(output)) {
		strcat(inode->data_blocks[id_last_tab]->data, output);
		printf("\n au passage, contenu du fichier : \n");
		myread(inode);
	}
	else { //we have to create another
		printf("\nError : not enough space available\n");
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

void df(Disk* disk) {
	int available = DISK_BYTES_LIMIT-(disk->nb_data_blocks*1024);
	
	printf("\n#### Informations about your disk : ####\n\n");
	printf("Nombre d'inodes utilisés : %d\n", disk->nb_inode);
	printf("Nombre de blocs de données utilisés : %d\n", disk->nb_data_blocks);
	printf("Nombre de blocs de répertoires utilisés : %d\n", disk->nb_dir_blocks);
	printf("Taille de l'espace disponible :  %d octets\n\n", available);
}

/* 

void ln(Inode** inode, Inode* current_inode,Disk* disk){
	int nbr_inode=sizeof*inode;
	
	if (nbr_inode==1){ //add in the current directory an index associated with this inode without changing the name of the file.
			
		update_tab_index(current_inode,*inode);
	}
	else if(nbr_inode==2){ //create a symbolic link (once again, an index) of the first inode given, and will give the name of the second one to the created index.
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
	}
}*/
