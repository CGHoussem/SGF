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
	Inode** inodes_input;
    char** parsedInput = parse(input);
    if (strcmp(input, "mkdir") == 0){
		//TODO créer plusieurs dossier avec un seul mkdir
		if(parsedInput[1] != NULL){
			mkdir(parsedInput[1], disk, disk->inodes);
		} else {
			printf("No directory name input \n");
		}
		free_input(input,parsedInput);
        return 1;
    
    } else if (strcmp(input, "ls") == 0){
        ls(current_inode);
        free_input(input,parsedInput);
        return 1;
    
    } else if (strcmp(input, "touch") == 0){
		//TODO gérer plusieurs fichiers + modification heure de modification
		if(parsedInput[1] != NULL){
			mycreate(parsedInput[1], disk, disk->inodes);
		} else {
			printf("No file name input \n");
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
		
		mv(inodes_input,nb_arg,disk);
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
		//TODO : gérer plusieurs suppressions, vérifier la récupération de l'inode
		nb_arg = count_path(parsedInput);
		if(nb_arg < 1) {
			printf("Missing file input \n");
			free_input(input,parsedInput);
			return 1;
		}
		
		inodes_input = (Inode**) malloc(nb_arg*sizeof(Inode*)); //input of move
		
		i = 1;
		cpt = 0;
		
		while(parsedInput[i] != NULL) {
			if(parsedInput[i][0] != '-') {
				inodes_input[cpt] = path_to_inode(parsedInput[i],current_inode,disk);
				if(inodes_input[cpt] == NULL) {
					printf("Error: argument %d is not an existing file \n",(cpt+1));
					free_input(input,parsedInput);
					free(inodes_input);
					return 1;
				} else if(inodes_input[cpt]->type == DIRECTORY) {
					printf("Error: argument %d is a directory \n",(cpt+1));
					free_input(input,parsedInput);
					free(inodes_input);
					return 1;
				}
				printf("suppression à venir : %s\n",parsedInput[i]);
				cpt++;
			}
			i++;
		}
		
		rm(inodes_input,nb_arg,disk);
		free(inodes_input);
		free_input(input,parsedInput);
        return 1;

    
    } else if (strcmp(input, "rmdir") == 0){
		//TODO : gérer plusieurs suppressions, vérifier la récupération de l'inode
		nb_arg = count_path(parsedInput);
		if(nb_arg < 1) {
			printf("Missing directory input \n");
			free_input(input,parsedInput);
			return 1;
		}
		
		inodes_input = (Inode**) malloc(nb_arg*sizeof(Inode*)); //input of move
		
		i = 1;
		cpt = 0;
		
		while(parsedInput[i] != NULL) {
			if(parsedInput[i][0] != '-') {
				inodes_input[cpt] = path_to_inode(parsedInput[i],current_inode,disk);
				if(inodes_input[cpt] == NULL) {
					printf("Error: argument %d is not an existing directory \n",(cpt+1));
					free_input(input,parsedInput);
					free(inodes_input);
					return 1;
				} else if(inodes_input[cpt]->type != DIRECTORY) {
					printf("Error: argument %d is a file \n",(cpt+1));
					free_input(input,parsedInput);
					free(inodes_input);
					return 1;
				}
				printf("suppression à venir : %s\n",parsedInput[i]);
				cpt++;
			}
			i++;
		}
		
		rmdir(inodes_input,nb_arg,disk);
		free(inodes_input);
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

		char digit;
		
		// verifying the integer input concerning the rights
		for(int i=0;i<length;i++) {
			digit = parsedInput[1][i];
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
		
		int rights = atoi(parsedInput[1]);
		printf ("Given input is a number : %d\n", rights);
		
		//files input of chmod
		inodes_input = (Inode**) malloc(nb_arg*sizeof(Inode*));
		
		i = 2;
		cpt = 0;
		
		while(parsedInput[i] != NULL) {
			if(parsedInput[i][0] != '-') {
				inodes_input[cpt] = path_to_inode(parsedInput[i],current_inode,disk);
				if(inodes_input[cpt] == NULL) {
					printf("Error: argument %d is not an existing file/directory \n",(cpt+1));
					free_input(input,parsedInput);
					free(inodes_input);
					return 1;
				}
				cpt++;
			}
			i++;
		}
		
		char permissions[9];
		strcpy(permissions, convertRights(rights, length, permissions));
		chmod(inodes_input,nb_arg-1,permissions,disk);
		free(inodes_input);
		free_input(input,parsedInput);
        return 1;
		
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

char* convertRights(int rights, int length, char permissions[9]) {

	int i;
	init_permissions(permissions);
	
	switch(length) {
		case 1:
			for(i=0;i<6;i++) permissions[i] = '-';
			permissions[6] = 'r';
			permissions[7] = 'w';
			permissions[8] = 'x';
			break;
		case 2:
			for(i=0;i<3;i++) permissions[i] = '-';
			permissions[3] = 'r';
			permissions[4] = 'w';
			permissions[5] = 'x';
			permissions[6] = 'r';
			permissions[7] = 'w';
			permissions[8] = 'x';
			break;
		case 3:
			permissions[0] = 'r';
			permissions[1] = 'w';
			permissions[2] = 'x';
			permissions[3] = 'r';
			permissions[4] = 'w';
			permissions[5] = 'x';
			permissions[6] = 'r';
			permissions[7] = 'w';
			permissions[8] = 'x';
			break;
		default:
			break;
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
			free_inode(disk,inode_create);
			return NULL;
		}	
	}
	return next_inode;

	/*printf("path to parent inode \n");

	file_name = strtok(parsedInput,"/");
	printf("1 %s \n",file_name);
	if(strcmp(file_name,"root") == 0){
		inode = disk->inodes;
		next_inode = inode;
	} else {
		next_inode = search_file_in_directory(file_name,inode->dir_blocks);
	}
	file_name = strtok(NULL,"/");
	printf("2 %s \n",file_name);
	while(file_name != NULL) {
		inode = next_inode;
		next_inode = search_file_in_directory(file_name,inode->dir_blocks);
		file_name = strtok(NULL,"/");
		printf("3 %s \n",file_name);
		if(next_inode == NULL && file_name != NULL) { //file doesn't exist
			printf("Error : path doesn't exist \n");
			return NULL;
		}
	}
	if(next_inode == NULL) {
		next_inode = mycreate(
		printf("inode: %p \n",inode);
		return inode;
	} else {
		printf("inode: %p \n",next_inode);
		return next_inode;
	}*/
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
