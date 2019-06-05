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
cp : pour copier un fichier dans un autre
rm : pour supprimer un fichier
mv : pour déplacer un fichier
cat : pour concaténer des fichiers
ln : pour créer un lien symbolique entre deux fichiers
echo “texte” > file : pour écrire dans un fichier
ls : pour afficher les éléments d’un répertoire
mkdir : pour créer un répertoire
rmdir : pour supprimer un répertoire
cd : pour changer de répertoire courant
df : pour avoir les infos du disque (nombre de blocs et d’inodes disponibles, et taille en octets de l’espace disponible)
*/

void mkdir(char* name,Disk* disk,Inode* current_inode){
	//TODO Vérifier que le dossier n'existe pas déjà
	Inode* inode = NULL;
	
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
	//TODO Vérifier que le fichier n'existe pas déjà
    Inode* inode = NULL;
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
		printf("%s -> file type : %s\n",current_inode->dir_blocks->tab_index[i].name, file_type);
	}

}

void cp(Inode** inodes,int number,Disk* disk){
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

void cd (char *name,Inode *current_inode)
{	
	Directory_block* directory;
	directory= current_inode->dir_blocks;
	if (search_file_in_directory(name,directory))
	{
		current_inode = search_file_in_directory(name,directory);
	}
	else 
	{
		printf("This file %s doesn't exist\n",name);
	}
}
		

/*
void mv(Inode source, Inode cible){
    
    if (source != cible){
        cp(source,cible);

    }
    
    cp(source,cible);
    rm(source.name, source);

}
*/
/*
void rm(char* name, Inode prev_inode){
    Inode inode;

	La fonction desalloc n'existe pas, il faut l'écrire (sinon c'est free la fonction) --Solenn
	inode.tab_block = (Block*) desalloc(sizeof(Block)); 
	
	inode.dir_blocks=allocation_tab_block_directory(1);

    // Il faut la variable disk pour la fonction init_block_directory --Houssem
    //init_block_directory(inode.dir_blocks, inode, prev_inode);
    
    inode.next_inode=NULL;
}
*/
