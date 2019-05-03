#include <stdio.h>
#include <stdlib.h>
#include "struct_SGF.h"
#include "primitives.h"
#include "command_shell.h"
#include "save_load_SGF.h"

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
    printf("\nDir: %s", cwd); 
} 
  


Inode mycreate(char* name,Inode prev_inode){  //creation d'un fichier
    Inode inode;

    strcpy(inode.name,name);
    init_permissions(inode.permissions);
    inode.type=1;  //text
    inode.date_creation=time(NULL);
    inode.date_modification=time(NULL);

    inode.tab_block = (Block*) malloc(sizeof(Block));
    inode.tab_block->b_directory=allocation_tab_block_directory;


	init_block_directory(inode.tab_block->b_directory,inode,prev_inode);
	inode.next_inode=NULL;


    return inode;

}

void cp(Inode source, Inode cible){
    strcpy(cible.name,source.name);
    init_permissions(cible.permissions);
    cible.type=source.type;


    cible.tab_block = (Block*) malloc(sizeof(Block));
    cible.tab_block->b_directory=allocation_tab_block_directory;


	init_block_directory(cible.tab_block->b_directory,cible,source);
	cible.next_inode=NULL;


}

void mv(Inode source, Inode cible){
    /*
    if (source != cible){
        cp(source,cible);

    }
    */
    cp(source,cible);
    rm(source.name, source);

}

void rm(char* name, Inode prev_inode){
    Inode inode;


	inode.tab_block = (Block*) desalloc(sizeof(Block));
	inode.tab_block->b_directory=allocation_tab_block_directory(1);
    init_block_directory(inode.tab_block->b_directory,inode,prev_inode);
    inode.next_inode=NULL;
}