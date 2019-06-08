#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//#include <unistd.h>
#include "shell_utility.h"
#include "command_shell.h"
#include "primitives.h"
#include "save_load_SGF.h"

char* readline(){ 
    char* input = malloc(sizeof(char) * SHELL_BUFFER_SIZE);
    int pos = 0;
    char c;
    while ((c = getchar()) != '\n' && c != EOF){
        input[pos++] = c;
    }
    input[pos] = '\0';
    return input;
}

char** parse(char* input){
    int i = 0;
    char** args = malloc(sizeof(char*) * SHELL_BUFFER_SIZE);
    const char* space = " ";
    char* arg = strtok(input, space);
    while (arg != NULL){
        args[i++] = arg;
        arg = strtok(NULL, space);
    }
    args[i] = NULL;
    return args;
}

int executeLine(Disk* disk, char* input,Inode* current_inode){
	int nb_arg,i,cpt;
	Inode** inodes_input = NULL;
	Inode* inode = NULL;
    char** parsedInput = parse(input);
    
    if (strcmp(input, "mkdir") == 0){
		nb_arg = count_path(parsedInput);
		if(nb_arg < 1) {
			printf("Missing file input \n");
			free_input(input,parsedInput);
			return 1;
		}
		
		i = 1;
		while(parsedInput[i] != NULL) {
			if(parsedInput[i][0] != '-') {
				if(path_to_destination_directory(parsedInput[i],current_inode,disk) == NULL) {
					printf("Error at argument %d, the directory was not created \n",i);
				} 
			}
			i++;
		}
		
		free_input(input,parsedInput);
        return 1;
    
    } else if (strcmp(input, "ls") == 0){
        ls(current_inode);
        free_input(input,parsedInput);
        return 1;
    
    } else if (strcmp(input, "touch") == 0){
		nb_arg = count_path(parsedInput);
		if(nb_arg < 1) {
			printf("Missing file input \n");
			free_input(input,parsedInput);
			return 1;
		}
		
		i = 1;
		while(parsedInput[i] != NULL) {
			if(parsedInput[i][0] != '-') {
				inode = path_to_destination(parsedInput[i],current_inode,disk);
				if(inode == NULL) {
					printf("Error at argument %d \n",i);
				} else {
					inode->date_modification = time(NULL);
				}
			}
			i++;
		}
		
		free_input(input,parsedInput);
        return 1;
    
    } else if (strcmp(input, "cp") == 0){
		nb_arg = count_path(parsedInput);
		if(nb_arg < 2) {
			printf("Missing file input \n");
			free_input(input,parsedInput);
			return 1;
		}
		
		inodes_input = (Inode**) malloc(nb_arg*sizeof(Inode*)); //input of cp
		
		i = 1;
		cpt = 0;
		
		while(parsedInput[i] != NULL) {
			if(parsedInput[i][0] != '-') {
				if(cpt == nb_arg-1) { //destination file
					inodes_input[cpt] = path_to_destination(parsedInput[i],current_inode,disk);
					if(inodes_input[cpt] == NULL) { //destination file doesn't exist
						free(inodes_input);
						free_input(input,parsedInput);
						return 1;
					}
					if(nb_arg != 2 && (inodes_input[cpt])->type != DIRECTORY) {
						printf("Error: More than a file to copy into a file \n");
						remove_tab_index(inodes_input[cpt],disk);
						free_inode(disk,inodes_input[cpt]);
						free(inodes_input);
						free_input(input,parsedInput);
						return 1;
					}
				} else { //source file
					inodes_input[cpt] = path_to_inode(parsedInput[i],current_inode,disk);
					if(inodes_input[cpt] == NULL) {
						printf("Error: argument %d is not an existing file \n",(cpt+1));
						free_input(input,parsedInput);
						free(inodes_input);
						return 1;
					}else if(inodes_input[cpt]->type == DIRECTORY) {
						printf("Error: argument %d is a directory \n",(cpt+1));
						free_input(input,parsedInput);
						free(inodes_input);
						return 1;
					}
				}
				cpt++;
			}
			i++;
		}
		cp(inodes_input,nb_arg,disk);
		free(inodes_input);
		free_input(input,parsedInput);
        return 1;
    
    } else if (strcmp(input, "mv") == 0){
		//TODO supprimer inode(s) source(s) ou les modifier uniquement
		nb_arg = count_path(parsedInput);
		if(nb_arg < 2) {
			printf("Missing file input \n");
			free_input(input,parsedInput);
			return 1;
		}
		
		inodes_input = (Inode**) malloc(nb_arg*sizeof(Inode*)); //input of move
		
		i = 1;
		cpt = 0;
		
		while(parsedInput[i] != NULL) {
			if(parsedInput[i][0] != '-') {
				if(cpt == nb_arg-1) { //destination file
					inodes_input[cpt] = path_to_destination(parsedInput[i],current_inode,disk);
					if(inodes_input[cpt] == NULL) { //destination file doesn't exist
						free(inodes_input);
						free_input(input,parsedInput);
						return 1;
					}
					if(nb_arg != 2 && (inodes_input[cpt])->type != DIRECTORY) {
						printf("Error: More than a file to move into a file \n");
						remove_tab_index(inodes_input[cpt],disk);
						free_inode(disk,inodes_input[cpt]);
						free(inodes_input);
						free_input(input,parsedInput);
						return 1;
					} 
				} else { //source file
					inodes_input[cpt] = path_to_inode(parsedInput[i],current_inode,disk);
					if(inodes_input[cpt] == NULL) {
						printf("Error: argument %d is not an existing file \n",(cpt+1));
						free_input(input,parsedInput);
						free(inodes_input);
						return 1;
					}else if(inodes_input[cpt]->type == DIRECTORY) {
						printf("Error: argument %d is a directory \n",(cpt+1));
						free_input(input,parsedInput);
						free(inodes_input);
						return 1;
					}
				}
				cpt++;
			}
			i++;
		}
		
		mymv(inodes_input,nb_arg,disk);
		free(inodes_input);
		free_input(input,parsedInput);
        return 1;
 
    
    } else if (strcmp(input, "cd") ==0){
		//TODO gérer cd hors du dossier courant
		if(parsedInput[1] != NULL){
			cd(parsedInput[1], current_inode,disk);
		}
		free_input(input,parsedInput);
		return 1;

    
    } else if (strcmp(input, "rm") ==0){
		nb_arg = count_path(parsedInput);
		if(nb_arg < 1) {
			printf("Missing file input \n");
			free_input(input,parsedInput);
			return 1;
		}
		
		i = 1;
		
		while(parsedInput[i] != NULL) {
			if(parsedInput[i][0] != '-') {
				inode = path_to_inode(parsedInput[i],current_inode,disk);
				if(inode == NULL) {
					printf("Error: argument %d is not an existing file \n",i);
				} 
				else if(inode->type == DIRECTORY) {
					printf("Error: argument %d is a directory \n",i);
				} 
				else {
					myrm(inode,disk);
				}
			}
			i++;
		}
		
		free_input(input,parsedInput);
        return 1;

    
    } else if (strcmp(input, "rmdir") == 0){
		nb_arg = count_path(parsedInput);
		if(nb_arg < 1) {
			printf("Missing directory input \n");
			free_input(input,parsedInput);
			return 1;
		}
		
		i = 1;
		
		while(parsedInput[i] != NULL) {
			if(parsedInput[i][0] != '-') {
				inode = path_to_inode(parsedInput[i],current_inode,disk);
				if(inode == NULL) {
					printf("Error: argument %d is not an existing directory \n",i);
				} 
				else if(inode->type != DIRECTORY) {
					printf("Error: argument %d is not a directory \n",i);
				}
				else if(inode->dir_blocks->nb_index > 2) {
					printf("Error: argument %d is not an empty directory \n",i);
				}
				else {
					myrm(inode,disk);
				}
			}
			i++;
		}
		
		free_input(input,parsedInput);
        return 1;
    
	
	} else if (strcmp(input, "chmod") == 0){
		
        nb_arg = count_path(parsedInput);
        
        if(nb_arg < 2) {
			printf("Missing input(s) \n");
			free_input(input,parsedInput);
			return 1;
		}
		
		int length = strlen(parsedInput[1]);
		
		// verifying that the given input is in the right range
		if(length < 1 || length > 3) {
			printf("Rights input is incorrect\n");
			free_input(input,parsedInput);
			return 1;
		}

		int digit;
		
		// verifying the integer input concerning the rights
		for(int i=0;i<length;i++) {
			digit = (int)parsedInput[1][i] - ASCII_OFFSET;
			if(isdigit(parsedInput[1][i]))
			{
				if(digit == 8 || digit == 9) {	
					printf("Rights input is incorrect\n");
					free_input(input,parsedInput);
					return 1;
				}
			}
			else {
				printf("Rights input is incorrect\n");
				free_input(input,parsedInput);
				return 1;
			}
		}
		
		printf ("Given input is a number : %s\n", parsedInput[1]);
		
		//files input of chmod
		inodes_input = (Inode**) malloc(nb_arg*sizeof(Inode*));
		
		i = 2;
		cpt = 0;
		
		while(parsedInput[i] != NULL) {
			if(parsedInput[i][0] != '-') {
				inodes_input[cpt] = path_to_inode(parsedInput[i],current_inode,disk);
				if(inodes_input[cpt] == NULL) {
					printf("Error: argument %d is not an existing file/directory \n",(cpt+2));
					free_input(input,parsedInput);
					free(inodes_input);
					return 1;
				}
				cpt++;
			}
			i++;
		}
		
		char permissions[9];
		strcpy(permissions, convertRights(parsedInput[1], length, permissions));
		mychmod(inodes_input,nb_arg-1,permissions,disk);
		free(inodes_input);
		free_input(input,parsedInput);
        return 1;
		
		return 1;
    
    } else if (strcmp(input, "df") == 0){
		//TODO : afficher plus d'infos (limites, mieux calculer l'espace restant)
        nb_arg = count_path(parsedInput);
        if(nb_arg > 0) {
			printf("df doesn't require arguments\n");
			free_input(input,parsedInput);
			return 1;
		}
		
		df(disk);
		
		return 1;
    
    } else if (strcmp(input, "help") == 0){
        printf("Available commands: mkdir, touch, ls, cp, mv, rm, exit\n");
        free_input(input,parsedInput);
        return 1;
   
    } else if (strcmp(input, "exit") == 0){
		free_input(input,parsedInput);
        return 0;
    
    } else {
        printf("%s: command not found\n", input);
        free_input(input,parsedInput);
        return 1;
    }
}

char* convertRights(char* rights, int length, char permissions[9]) {

	int rights_user, rights_group, rights_other;

	delete_permissions(permissions);

	switch(length) {
		case 1:
			rights_other = (int)rights[0] - ASCII_OFFSET;
			break;
		case 2:
			
			rights_group = (int)rights[0] - ASCII_OFFSET;
			rights_other = (int)rights[1] - ASCII_OFFSET;
			
			if(rights_group > 0) {
				if(rights_group >= 4) permissions[3] = 'r';
				if(rights_group%6 == 0 || rights_other == 7) permissions[4] = 'w';
				if(rights_group%2 != 0) permissions[5] = 'x';
			}
			
			break;
			
		case 3:
			
			rights_user = (int)rights[0] - ASCII_OFFSET;
			rights_group = (int)rights[1] - ASCII_OFFSET;
			rights_other = (int)rights[2] - ASCII_OFFSET;
			//printf("%d %d %d", rights_user, rights_group, rights_other);
			
			if(rights_user > 0) {
				if(rights_user >= 4) permissions[0] = 'r';
				if(42%rights_user == 0 && rights_user != 1) permissions[1] = 'w';
				if(rights_user%2 != 0) permissions[2] = 'x';
			}
			
			if(rights_group > 0) {
				if(rights_group >= 4) permissions[3] = 'r';
				if(42%rights_group == 0 && rights_group != 1) permissions[4] = 'w';
				if(rights_group%2 != 0) permissions[5] = 'x';
			}
			
			break;
		
	}
	
	if(rights_other > 0) {
		if(rights_other >= 4) permissions[6] = 'r';
		if(42%rights_other == 0 && rights_other != 1) permissions[7] = 'w';
		if(rights_other%2 != 0) permissions[8] = 'x';
	}	
	
	return permissions;
}

Inode* path_to_inode(char* parsedInput,Inode* current_inode,Disk* disk){	
	char* file_name;
	Inode* inode = NULL;
	
	if(parsedInput[0] == '/') {
		inode = disk->inodes;
	} else {
		inode = current_inode;
	}
	
	for(file_name = strtok(parsedInput,"/");file_name != NULL;file_name = strtok(NULL,"/")){
		inode = search_file_in_directory(file_name,inode->dir_blocks);
		if(inode == NULL) { //file doesn't exist
			return NULL;
		}
	}

	return inode;
}

Inode* path_to_destination(char* parsedInput,Inode* current_inode,Disk* disk){	
	char* file_name;
	Inode* inode = NULL;
	Inode* next_inode = NULL;
	Inode* inode_create = NULL;
	int create = 0;
	
	if(parsedInput[0] == '/') {
		next_inode = disk->inodes;
	} else {
		next_inode = current_inode;
	}
	
	for(file_name = strtok(parsedInput,"/");file_name != NULL;file_name = strtok(NULL,"/")){
		inode = next_inode;
		next_inode = search_file_in_directory(file_name,inode->dir_blocks);
		if(next_inode == NULL && create == 0) {
			mycreate(file_name,disk,inode);
			inode_create = get_last_inode(*disk);
			next_inode = inode_create;
			create = 1;
		} else if(next_inode == NULL && create == 1) {
			printf("Error: Destination path doesn't exist \n");
			remove_tab_index(inode_create,disk);
			free_inode(disk,inode_create);
			return NULL;
		}	
	}
	return next_inode;
}

Inode* path_to_destination_directory(char* parsedInput,Inode* current_inode,Disk* disk){	
	char* file_name;
	Inode* inode = NULL;
	Inode* next_inode = NULL;
	Inode* inode_create = NULL;
	int create = 0;
	
	if(parsedInput[0] == '/') {
		next_inode = disk->inodes;
	} else {
		next_inode = current_inode;
	}
	
	for(file_name = strtok(parsedInput,"/");file_name != NULL;file_name = strtok(NULL,"/")){
		inode = next_inode;
		next_inode = search_file_in_directory(file_name,inode->dir_blocks);
		if(next_inode == NULL && create == 0) {
			mkdir(file_name,disk,inode);
			inode_create = get_last_inode(*disk);
			next_inode = inode_create;
			create = 1;
		} else if(next_inode == NULL && create == 1) {
			printf("Error: Path doesn't exist \n");
			remove_tab_index(inode_create,disk);
			free_inode(disk,inode_create);
			return NULL;
		}	
	}
	if(create == 0) {
		printf("Error: The file %s already exist in this directory \n",next_inode->name);
	}
	return next_inode;
}

int count_path(char** parsedInput) {
	int i = 1;
	int nb_arg = 0;
	
	while(parsedInput[i] != NULL) {
		if(parsedInput[i][0] != '-') {
			nb_arg++;
		}
		i++;
	}
	return nb_arg;
}

void free_input(char* input,char** parsedInput) {
	free(input);
	free(parsedInput);
}
