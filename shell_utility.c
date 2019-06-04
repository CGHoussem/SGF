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
		if(nb_arg == 0) {
			printf("Error : no file name input \n");
			return 1;
		}
		inodes_input = (Inode**) malloc(nb_arg*sizeof(Inode*));
		i = 1;
		cpt = 0;
		while(parsedInput[i] != NULL) {
			if(parsedInput[i][0] != '-') {
				if(cpt == nb_arg-1) {
					inodes_input[cpt] = path_to_last_directory(parsedInput[i],current_inode,disk);
				} else {
					inodes_input[cpt] = path_to_inode(parsedInput[i],current_inode,disk);
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
	Inode* inode = current_inode;
	
	printf("path to inode \n");

	file_name = strtok(parsedInput,"/");
	printf("1 %s \n",file_name);
	if(strcmp(file_name,"root") == 0){
		inode = disk->inodes;
	} else {
		inode = search_file_in_directory(file_name,inode->dir_blocks);
	}
	file_name = strtok(NULL,"/");
	printf("2 %s \n",file_name);
	while(file_name != NULL) {
		inode = search_file_in_directory(file_name,inode->dir_blocks);
		file_name = strtok(NULL,"/");
		printf("3 %s \n",file_name);
		if(inode == NULL) { //file doesn't exist
			printf("inode: %p \n",inode);
			return NULL;
		}
	}
	printf("inode: %p \n",inode);
	return inode;
}

Inode* path_to_last_directory(char* parsedInput,Inode* current_inode,Disk* disk){	
	char* file_name;
	Inode* inode = current_inode;
	Inode* next_inode = NULL;

	printf("path to parent inode \n");

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
		printf("inode: %p \n",inode);
		return inode;
	} else {
		printf("inode: %p \n",next_inode);
		return next_inode;
	}
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
