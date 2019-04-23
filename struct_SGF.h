#ifndef STRUCT_SGF
#define STRUCT_SGF

typedef struct 
{
	char permissions[9] = {1,1,1,1,0,0,1,0,0};  // rwxr--r--
	int typefichier; // 1 = texte, 2 = binaire, 3 = repertoire
	time_t date_creation;
	time_t date_modification;
	bloc bloc;
	inode* inode_suivant;
} inode;

typedef union{
	struct bloc_donnees;
	struct bloc_rep;
} bloc;

typedef struct{
	char* nom;
	inode* inode;
} index;

typedef struct {
	char donnees[1024];
	int taille_fichier;
} bloc_donnees;

typedef struct {
index* index;
} bloc_rep;

typedef struct{
	inode* inodes;	//1er inode
	int nb_inode;
	bloc* blocs; //1er bloc
	int nb_bloc;
} disque;


#endif
