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
	
	init_block_data(inode->data_blocks,inode,current_inode,disk,name);
	update_tab_index(current_inode,inode);
		
	add_inode(inode,disk);
	
	printf("The file %s has been created successfully ! \n",name);
	
}

void ls(Disk* disk, int index) {
	Index rep_courant = disk->dir_blocks->tab_index[index];
	Inode* inode = rep_courant.inode;
	
	while(inode != NULL && inode->next_inode != NULL) {
		printf("%s\n", inode->name);
		inode = inode->next_inode;
	}
}

void cp(Inode source, Inode cible){
    strcpy(cible.name,source.name);
    init_permissions(cible.permissions);
    cible.type=source.type;


	/* pareil qu'au dessus  --Solenn

    cible.tab_block = (Block*) malloc(sizeof(Block));
    cible.tab_block->b_directory=allocation_tab_block_directory;


	init_block_directory(cible.tab_block->b_directory,cible,source);
	cible.next_inode=NULL; */


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
