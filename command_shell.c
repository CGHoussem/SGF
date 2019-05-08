#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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


// Function to print Current Directory. 
void printDir() 
{ 
    char cwd[1024]; 
     
    getcwd(cwd, sizeof(cwd)); 
    printf("\n%s:%s$:", getenv("USER"), cwd); 
} 
 
void ls(Disk* disk, int index) {
	Index rep_courant = disk->dir_blocks->tab_index[index];
	Inode* inode = rep_courant.inode;
	
	while(inode != NULL && inode->next_inode != NULL) {
		printf("%s\n", inode->name);
		inode = inode->next_inode;
	}
}

void mycreate(Disk* disk, int index, char* name){  //creation d'un fichier
    Index rep_courant = disk->dir_blocks->tab_index[index];
    Inode* inode = rep_courant.inode;
	Inode* courant = rep_courant.inode;

    strcpy(inode->name, name);
    init_permissions(inode->permissions);
    inode->type=1;
    inode->date_creation=time(NULL);
    inode->date_modification=time(NULL);
	
	while(courant != NULL && courant->next_inode != NULL) {
		courant = courant->next_inode;
	}

	// adding the inode at the end of the list
	courant->next_inode = inode;
	inode->next_inode = NULL;
	
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
