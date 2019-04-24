#ifndef STRUCT_SGF
#define STRUCT_SGF

#include <time.h>

typedef struct index index;
typedef struct bloc_donnees bloc_donnees;
typedef struct bloc_repertoire bloc_repertoire;
typedef union bloc bloc;
typedef struct inode inode;
typedef struct disque disque;

struct index {
	char* nom;
	inode* inode;
};

struct bloc_donnees {
	char donnees[1024];
	int taille_fichier;
};

struct bloc_repertoire {
	index* index;
};

union bloc {
	bloc_donnees b_donnees;
	bloc_repertoire b_repertoire;
};

struct inode {
	char permissions[9]; // rwxr--r-- {1,1,1,1,0,0,1,0,0}
	int typefichier; // 1 = texte, 2 = binaire, 3 = repertoire
	time_t date_creation;
	time_t date_modification;
	bloc bloc;
	inode* inode_suivant;
};

struct disque {
	inode* inodes;	//1er inode
	int nb_inode;
	bloc* blocs; //1er bloc
	int nb_bloc;
};

#endif
