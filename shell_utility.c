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

int executeLine(Disk disk, char* input,Inode* current_inode){
	int nb_arg = 0;
	Inode** inodes_input;
    char** parsedInput = parse(input);
    if (strcmp(input, "mkdir") == 0){
		if(parsedInput[1] != NULL){
			mkdir(parsedInput[1], &disk, disk.inodes);
		} else {
			printf("No directory name input \n");
		}
        return 1;
    } else if (strcmp(input, "ls") == 0){
        ls(disk.inodes);
        return 1;
    } else if (strcmp(input, "touch") == 0){
		if(parsedInput[1] != NULL){
			mycreate(parsedInput[1], &disk, disk.inodes);
		} else {
			printf("No file name input \n");
		}
        return 1;
    } else if (strcmp(input, "cp") == 0){
        inodes_input = path_to_inode(parsedInput,&nb_arg,current_inode,&disk);
        if(inodes_input != NULL){
			cp(inodes_input,nb_arg,&disk);
			free(inodes_input);
		}
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

Inode** path_to_inode(char** parsedInput,int* nb_arg,Inode* current_inode,Disk* disk){	
	int i = 1;
	int cpt = 0;
	*nb_arg = 0;
	Inode** inodes;
	char* file_name;
	Inode* inode = current_inode;
	
	while(parsedInput[i] != NULL) {
		if(parsedInput[i][0] != '-'){
			(*nb_arg)++;
		}
		i++;
	}

	inodes = (Inode**) malloc(*nb_arg * sizeof(Inode*));
	
	i = 1;
	while(parsedInput[i] != NULL) {
		if(parsedInput[i][0] != '-'){
			file_name = strtok(parsedInput[i],"/");
			printf("%s \n",file_name);
			if(strcmp(file_name,"root") == 0){
				inode = disk->inodes;
			}
			file_name = strtok(NULL,"/");
			printf("%s \n",file_name);
			while(file_name != NULL) {
				inode = search_file_in_directory(file_name,inode->dir_blocks);
				if(inode == NULL) {
					printf("Error: argument %d inexisting path",i);
					free(inodes);
					return NULL;
				}
				file_name = strtok(NULL,"/");
				printf("%s \n",file_name);
			}
			inodes[cpt] = inode;
			cpt++;
		}
		i++;
	}
	return inodes;
}
