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
cat : pour concaténer des fichiers
ln : pour créer un lien symbolique entre deux fichiers
echo “texte” > file : pour écrire dans un fichier
df : pour avoir les infos du disque (nombre de blocs et d’inodes disponibles, et taille en octets de l’espace disponible)
ln: creer liens symboliques
*/
//TODO vérifier que le disque ne soit pas complet avant d'ajouter

void mkdir(char* name,Disk* disk,Inode* current_inode){
	Inode* inode = NULL;
	
	if(current_inode != NULL && search_file_in_directory(name,current_inode->dir_blocks) != NULL) {
		printf("Error: The file already exist in this directory \n");
		return;
	}
	inode = (Inode*)malloc(sizeof(Inode));
	
	strcpy(inode->name, name);
	
	init_permissions(inode->permissions);
	inode->type=DIRECTORY; 
	inode->date_creation=time(NULL);
	inode->date_modification=time(NULL);
	
	inode->data_blocks = NULL;
	inode->dir_blocks = allocation_tab_block_directory(1);

	inode->next_inode = NULL;
	
	if(current_inode == NULL){ //root
		init_block_directory(inode->dir_blocks,inode,inode,disk);
	}
	else{
		init_block_directory(inode->dir_blocks,inode,current_inode,disk);
		update_tab_index(current_inode,inode);
	}
	
	add_inode(inode,disk);
	
	printf("The directory %s has been created successfully ! \n",name);
	
}

void mycreate(char* name,Disk* disk,Inode* current_inode){
    Inode* inode = NULL;
    
    if(search_file_in_directory(name,current_inode->dir_blocks) != NULL) {
		printf("Error: The file already exist in this directory \n");
		return;
	}
	
	inode = (Inode*)malloc(sizeof(Inode));
	
	strcpy(inode->name, name);
	
	init_permissions(inode->permissions);
	inode->type=TEXT; 
	inode->date_creation=time(NULL);
	inode->date_modification=time(NULL);
	
	inode->data_blocks = allocation_tab_block_data(1);
	inode->dir_blocks = NULL;

	inode->next_inode = NULL;
	
	init_block_data(inode->data_blocks,disk);
	update_tab_index(current_inode,inode);
		
	add_inode(inode,disk);

	printf("The file %s has been created successfully ! \n",name);
	
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
		
		if(dest->data_blocks->size != 0) {
			for(j=0;j<dest->data_blocks->size;j++){ //delete the old data
				dest->data_blocks->data[i] = 0;
			}
		}
		
		dest->data_blocks->size = source->data_blocks->size;
		for(j=0;j<dest->data_blocks->size;j++){ //write the new data
			dest->data_blocks->data[i] = source->data_blocks->data[i];
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

void mv(Inode** inodes,int number,Disk* disk){
    
    int i,j;
	Inode* source = NULL;
	Inode* dest = NULL;
	
	for(i=0;i<number-2;i++) {
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
		
		//delete the old inode (which corresponds to the old location)
		remove_tab_index(source,disk);
		free_inode(disk,source);

		//copy the inode's informations
		for(j=0;j<9;j++){
			dest->permissions[i] = source->permissions[i];
		}
		dest-> date_modification = time(NULL);
		
		if(dest->data_blocks->size != 0) {
			for(j=0;j<dest->data_blocks->size;j++){ //delete the old data
				dest->data_blocks->data[i] = 0;
			}
		}
		
		dest->data_blocks->size = source->data_blocks->size;
		for(j=0;j<dest->data_blocks->size;j++){ //write the new data
			dest->data_blocks->data[i] = source->data_blocks->data[i];
		}
	}
}

void rm(Inode** inodes,int number,Disk* disk){
	int i;
	printf("%d fichiers à supprimer\n", number);
	for(i=0;i<number;i++) {
		printf("%s\n", inodes[i]->name);
		if(inodes[i] != NULL) { //file does exist
			remove_tab_index(inodes[i],disk);
			free_inode(disk,inodes[i]);
		}
		else { //file doesn't exist
			printf("The file %s doesn't exist here ! \n", inodes[i]->name);
		}
	}		
}

void rmdir(Inode** inodes,int number,Disk* disk){
	
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

void chmod(Inode** inodes,int number,char permissions[9],Disk* disk){
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
