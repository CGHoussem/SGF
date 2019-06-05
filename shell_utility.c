#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shell_utility.h"
#include "command_shell.h"
#include "primitives.h"
#include "save_load_SGF.h"

char* readline(){ 
    char* input = malloc(sizeof(char) * SHELL_BUFFER_SIZE); //inutile de faire un malloc avec macro constante, c'est pas dynamique
    //tableau statique + scanf est suffisant
    //ou calcul nombre de caractere avec getchar et après tableau dynamique
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
    char** args = malloc(sizeof(char) * SHELL_BUFFER_SIZE); // pareil, malloc avec macro inutile
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
		if(parsedInput[1] != NULL){
			mkdir(parsedInput[1], disk, disk->inodes);
		} else {
			printf("No directory name input \n");
		}
        return 1;
    
    } else if (strcmp(input, "ls") == 0){
        ls(disk->inodes);
        return 1;
    
    } else if (strcmp(input, "touch") == 0){
		if(parsedInput[1] != NULL){
			mycreate(parsedInput[1], disk, disk->inodes);
		} else {
			printf("No file name input \n");
		}
        return 1;
    
    } else if (strcmp(input, "cp") == 0){
		nb_arg = count_path(parsedInput);
		if(nb_arg < 2) {
			printf("Missing file input \n");
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
						return 1;
					}
					if(nb_arg != 2 && (inodes_input[cpt])->type != DIRECTORY) {
						printf("Error: More than a file to copy into a file \n");
						free_inode(disk,inodes_input[cpt]);
						free(inodes_input);
						return 1;
					} 
				} else { //source file
					inodes_input[cpt] = path_to_inode(parsedInput[i],current_inode,disk);
					if(inodes_input[cpt] == NULL) {
						printf("Error: argument %d is not an existing file \n",(cpt+1));
						free(inodes_input);
						return 1;
					}else if(inodes_input[cpt]->type == DIRECTORY) {
						printf("Error: argument %d is a directory \n",(cpt+1));
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
        return 1;
    
    } else if (strcmp(input, "mv") == 0){
        printf("IN DEV..\n");
        return 1;
    } else if (strcmp(input, "cd") ==0){
		if(parsedInput[1] != NULL){
			cd(parsedInput[1], current_inode,disk);
			}
	return 1;

    
    } else if (strcmp(input, "rm") == 0){
        printf("IN DEV..\n");
        return 1;
    
    } else if (strcmp(input, "help") == 0){
        printf("Available commands: mkdir, touch, ls, cp, mv, rm, exit\n");
        return 1;
   
    } else if (strcmp(input, "exit") == 0){
        return 0;
    
    } else {
        printf("%s: command not found\n", input);
        return 1;
    }
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
