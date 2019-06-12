#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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

int executeLine(Disk* disk, char* input,Inode** current_inode){
	int nb_arg,i,j,cpt;
	Inode** inodes_input = NULL;
	Inode* inode = NULL;
	Inode** inode_and_parent = NULL;
	Inode** parent_inodes_input = NULL;
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
				if(path_to_destination_directory(parsedInput[i],*current_inode,disk) == NULL) {
					printf("Error at argument %d, the directory was not created \n",i);
				} 
			}
			i++;
		}
		
		free_input(input,parsedInput);
        return 1;
    
    } else if (strcmp(input, "ls") == 0){
		nb_arg = count_path(parsedInput);
		if(nb_arg < 1) {
			for(j=0;j<(*current_inode)->dir_blocks->nb_index;j++) {
				myls((*current_inode)->dir_blocks->tab_index[j].inode,(*current_inode)->dir_blocks->tab_index[j].name);
			}
			free_input(input,parsedInput);
			return 1;
		}
		
		i = 1;
		while(parsedInput[i] != NULL) {
			if(parsedInput[i][0] != '-') {
				inode = path_to_inode(parsedInput[i],*current_inode,disk);
				if(inode == NULL) {
					printf("Error at argument %d : path doesn't exist \n",i);
				} else if(inode->type != DIRECTORY) {
					myls(inode,NULL);
				} else {
					printf("%s : \n",inode->name);
					for(j=0;j<inode->dir_blocks->nb_index;j++) {
						myls(inode->dir_blocks->tab_index[j].inode,inode->dir_blocks->tab_index[j].name);
					}
				}
			}
			i++;
		}
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
				inode = path_to_destination(parsedInput[i],*current_inode,disk);
				if(inode == NULL) {
					printf("Error at argument %d \n",i+1);
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
					inode_and_parent = path_to_destination_and_parent(parsedInput[i],*current_inode,disk);
					inodes_input[cpt] = inode_and_parent[0];
					if(inodes_input[cpt] == NULL) { //destination file doesn't exist
						free(inodes_input);
						free_input(input,parsedInput);
						return 1;
					}
					if(nb_arg != 2 && (inodes_input[cpt])->type != DIRECTORY) {
						printf("Error: More than a file to copy into a file \n");
						remove_tab_index(inodes_input[cpt],inode_and_parent[1],disk);
						free_inode(disk,inodes_input[cpt]);
						free(inodes_input);
						free_input(input,parsedInput);
						return 1;
					}
				} else { //source file
					inodes_input[cpt] = path_to_inode(parsedInput[i],*current_inode,disk);
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
		mycp(inodes_input,inode_and_parent[1],nb_arg,disk);
		free(inode_and_parent);
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
		parent_inodes_input = (Inode**) malloc(nb_arg*sizeof(Inode*));
		
		i = 1;
		cpt = 0;
		
		while(parsedInput[i] != NULL) {
			if(parsedInput[i][0] != '-') {
				if(cpt == nb_arg-1) { //destination file
					inode_and_parent = path_to_destination_and_parent(parsedInput[i],*current_inode,disk);
					inodes_input[cpt] = inode_and_parent[0];
					parent_inodes_input[cpt] = inode_and_parent[1];
					if(inodes_input[cpt] == NULL) { //destination file doesn't exist
						free(inodes_input);
						free(parent_inodes_input);
						free(inode_and_parent);
						free_input(input,parsedInput);
						return 1;
					}
					if(nb_arg != 2 && (inodes_input[cpt])->type != DIRECTORY) {
						printf("Error: More than a file to move into a file \n");
						remove_tab_index(inodes_input[cpt],parent_inodes_input[cpt],disk);
						free_inode(disk,inodes_input[cpt]);
						free(inodes_input);
						free(parent_inodes_input);
						free(inode_and_parent);
						free_input(input,parsedInput);
						return 1;
					} 
				} else { //source file
					inode_and_parent = path_to_inode_and_parent_inode(parsedInput[i],*current_inode,disk);
					inodes_input[cpt] = inode_and_parent[0];
					parent_inodes_input[cpt] = inode_and_parent[1];
					if(inodes_input[cpt] == NULL) {
						printf("Error: argument %d is not an existing file \n",(cpt+1));
						free_input(input,parsedInput);
						free(inodes_input);
						free(parent_inodes_input);
						free(inode_and_parent);
						return 1;
					}else if(inodes_input[cpt]->type == DIRECTORY) {
						printf("Error: argument %d is a directory \n",(cpt+1));
						free_input(input,parsedInput);
						free(inodes_input);
						free(parent_inodes_input);
						free(inode_and_parent);
						return 1;
					}
				}
				cpt++;
				free(inode_and_parent);
			}
			i++;
		}
		
		mymv(inodes_input,parent_inodes_input,nb_arg,disk);
		free(inodes_input);
		free(parent_inodes_input);
		free_input(input,parsedInput);
        return 1;
 
    
    } else if (strcmp(input, "cd") == 0){
		if(parsedInput[1] != NULL){
			inode = path_to_inode(parsedInput[1],*current_inode,disk);
			if(inode != NULL && inode->type == DIRECTORY) {
				mycd(inode, current_inode);
			} else {
				printf("Error : directory doesn't exist \n");
			}
		} else {
			printf("Missing input \n");
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
				inode_and_parent = path_to_inode_and_parent_inode(parsedInput[i],*current_inode,disk);
				if(inode_and_parent[0] == NULL) {
					printf("Error: argument %d is not an existing file \n",i+1);
				} 
				else if(inode_and_parent[0]->type == DIRECTORY) {
					printf("Error: argument %d is a directory \n",i+1);
				} 
				else {
					myrm(inode_and_parent[0],inode_and_parent[1],disk);
				}
				free(inode_and_parent);
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
				inode_and_parent = path_to_inode_and_parent_inode(parsedInput[i],*current_inode,disk);
				if(inode_and_parent[0] == NULL) {
					printf("Error: argument %d is not an existing directory \n",i+1);
				} 
				else if(inode_and_parent[0]->type != DIRECTORY) {
					printf("Error: argument %d is not a directory \n",i+1);
				}
				else if(inode_and_parent[0]->dir_blocks->nb_index > 2) {
					printf("Error: argument %d is not an empty directory \n",i+1);
				}
				else if(inode_and_parent[0] == *current_inode) {
					printf("Error: cannot delete the current directory \n");
				}
				else {
					myrm(inode_and_parent[0],inode_and_parent[1],disk);
				}
				free(inode_and_parent);
			}
			i++;
		}
		
		free_input(input,parsedInput);
        return 1;
    
	
	} else if (strcmp(input, "cat") == 0){
        nb_arg = count_path(parsedInput);
		if(nb_arg < 1) {
			printf("Error : missing input \n");
			free_input(input,parsedInput);
			return 1;
		}
		
		int i;
		//TODO Déclarations en haut
				
		int hasRedirection = 0;
		int redirectionIndex = 0;
		// checks if there is redirection
		for(i=1;i<=nb_arg;i++) {
			if(strcmp(parsedInput[i], ">") == 0) 
			{
				// verifying if there's a file after the redirection
				if(i == nb_arg || strcmp(parsedInput[i+1], "cat") == 0) {
					printf("Error : missing or bad file input \n");
					free_input(input,parsedInput);
					return 1;
				}
				//////TODO : other verifications
				redirectionIndex = i;
				hasRedirection = 1;
				break;
			}
		}
		
		// case of a simple output in the shell
		if(!hasRedirection) {
			for(i=1;i<=nb_arg;i++) {
				inode = path_to_inode(parsedInput[i],*current_inode,disk);
				if(inode == NULL) {
					printf("Error: argument %d is not an existing file \n",i+1);
				} 
				else if(inode->type != TEXT) {
					printf("Error: argument %d is not a file \n",i+1);
				}
				else {
					char* output;
					output = (char*) malloc(BUFFER_SIZE*inode->nb_data_blocks*sizeof(char));
					printf("%s\n", myread(inode, output));
					free(output);
				}
			}
			free_input(input,parsedInput);
			return 1;
		}
		
		int content_inode_size = 0;
		//int final size;
		//int* reallocation;
		char content_inodes[MAX_REDIRECTION_SIZE] = "";

		// First part of inodes (before the redirection char)
		for(i=1;i<redirectionIndex;i++) {
			inode = path_to_inode(parsedInput[i],*current_inode,disk);
			if(inode == NULL) {
				printf("Error: argument %d is not an existing file \n",i+1);
				free_input(input,parsedInput);
				return 1;
			} 
			else if(inode->type != TEXT) {
				printf("Error: argument %d is not a file \n",i+1);
				free_input(input,parsedInput);
				return 1;
			}
			else {
				char* content_inode;
				content_inode_size = BUFFER_SIZE*inode->nb_data_blocks;
				content_inode = (char*) malloc(content_inode_size*sizeof(char));
				/*content_inode_size = BUFFER_SIZE*inode->nb_data_blocks;
				final_size += content_inode_size;
				reallocation = realloc(content_inodes, final_size*sizeof(char));
				if(reallocation == NULL) { // the realloc hasn't worked
					printf("Error while allocating the new data blocks.\n");
					free(content_inodes);
					free_input(input,parsedInput);
					return 1;
				}*/
				content_inode = myread(inode, content_inode);
				strcat(content_inodes, content_inode);
			}
		}
		
		// Second part of the files which will be displayed (after the file which will be modified)
		if(nb_arg > redirectionIndex+1) {
			for(i=redirectionIndex+2;i<=nb_arg;i++) {
				for(i=1;i<redirectionIndex;i++) {
					inode = path_to_inode(parsedInput[i],*current_inode,disk);
					if(inode == NULL) {
						printf("Error: argument %d is not an existing file \n",i+1);
						free_input(input,parsedInput);
						return 1;
					} 
					else if(inode->type != TEXT) {
						printf("Error: argument %d is not a file \n",i+1);
						free_input(input,parsedInput);
						return 1;
					}
					else {
						char* content_inode;
						content_inode_size = BUFFER_SIZE*inode->nb_data_blocks;
						content_inode = (char*) malloc(content_inode_size*sizeof(char));
						/*
						final_size += content_inode_size;
						reallocation = realloc(content_inodes, final_size*sizeof(char));
						if(reallocation == NULL) { // the realloc hasn't worked
							printf("Error while allocating the new data blocks.\n");
							free(content_inodes);
							free_input(input,parsedInput);
							return 1;
						}*/
						content_inode = myread(inode, content_inode);
						strcat(content_inodes, content_inode);
					}
				}
			}
		}		
				
		inode = path_to_inode(parsedInput[redirectionIndex+1],*current_inode,disk);
		if(inode == NULL) {
			printf("Error: argument %d is not an existing file \n",redirectionIndex+1);
		} 
		else if(inode->type != TEXT) {
			printf("Error: argument %d is not a file \n",i+1);
		}
		else {
			mywrite(inode,content_inodes,disk);
		}
				
		free_input(input,parsedInput);
        return 1;
	
	} else if (strcmp(input, "echo") == 0){
        nb_arg = count_path(parsedInput);
		if(nb_arg < 1) {
			printf("Error : missing inputs \n");
			free_input(input,parsedInput);
			return 1;
		}
		
		int i,j;
	
		// counting quotes
		int nb_quotes = 0;
		for(i=1;i<=nb_arg;i++)
			for(j=0;j<strlen(parsedInput[i]);j++)
				nb_quotes += (parsedInput[i][j] == '"');
		
		// checks if quotes are opened but not closed
		if(nb_quotes%2 != 0) {
			printf("Error : you have entered %d quotes, 1 missing quote\n", nb_quotes);
			free_input(input,parsedInput);
			return 1;
		}
		
		int hasRedirection = 0;
		int redirectionIndex = 0;
		// checks if there are redirections
		for(i=1;i<=nb_arg;i++) {
			if(strcmp(parsedInput[i], ">") == 0) 
			{
				// verifying if there's a file after the redirection
				if(i == nb_arg) {
					printf("Error : missing file input \n");
					free_input(input,parsedInput);
					return 1;
				}
				redirectionIndex = i;
				hasRedirection = 1;
				break;
			}
		}
		
		// case of a simple output in the shell
		if(!hasRedirection) {
			for(i=1;i<=nb_arg;i++) {
				for(j=0;j<strlen(parsedInput[i]);j++)
					if(parsedInput[i][j] != '"')
						printf("%c", parsedInput[i][j]);
				printf(" ");
			}
			printf("\n");
			free_input(input,parsedInput);
			return 1;
		}
		
		
		char output[MAX_INPUT_SIZE] = "";
		int taille_totale;
		// First part of the array (before the redirection char)
		for(i=1;i<redirectionIndex;i++) {
			for(j=0;j<strlen(parsedInput[i]);j++) {
				taille_totale = strlen(output);
				if(taille_totale < MAX_INPUT_SIZE-1) {
					if(parsedInput[i][j] != '"')
						output[strlen(output)] = parsedInput[i][j];
					}
					else {
						printf("Error : size of the input is too high\n");
						free_input(input,parsedInput);
						return 1;
					}
			}
			output[strlen(output)] = ' ';
		}
		
		// Second part of the array (after the file which will be modified)
		if(nb_arg > redirectionIndex+1) {
			for(i=redirectionIndex+2;i<=nb_arg;i++) {
				for(j=0;j<strlen(parsedInput[i]);j++) {
					taille_totale = strlen(output);
					if(taille_totale < MAX_INPUT_SIZE-1) {
						if(parsedInput[i][j] != '"')
							output[strlen(output)] = parsedInput[i][j];
					}
					else {
						printf("Error : size of the input is too high\n");
						free_input(input,parsedInput);
						return 1;
					}
				}
				output[strlen(output)] = ' ';
			}
		}
		output[strlen(output)] = '\0';
		
		printf("string to redirect : \n%s\n", output);
		
		inode = path_to_inode(parsedInput[redirectionIndex+1],*current_inode,disk);
		if(inode == NULL) {
			inode = path_to_destination(parsedInput[redirectionIndex+1],*current_inode,disk);
			if(inode == NULL) {
				printf("Error at argument %d \n",i+1);
				free_input(input,parsedInput);
				return 1;
					
			} else {
				inode->date_modification = time(NULL);
				mywrite(inode,output,disk);
			}
		} 
		else if(inode->type != TEXT) {
			printf("Error: argument %d is not a file \n",i+1);
		}
		else {
			mywrite(inode,output,disk);
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
				inodes_input[cpt] = path_to_inode(parsedInput[i],*current_inode,disk);
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
		
		mydf(disk);
		
		return 1;
		
	} else if (strcmp(input, "ln") == 0){
		nb_arg = count_path(parsedInput);
		if(nb_arg < 1) {
			printf("Error: Missing file input \n");
			free_input(input,parsedInput);
			return 1;
		}

		inodes_input = (Inode**) malloc(nb_arg * sizeof(Inode*));
		
		i = 1;
		cpt = 0;
		while(parsedInput[i] != NULL) {
			if(parsedInput[i][0] != '-') {
				if(nb_arg > 1 && cpt == nb_arg-1) {
					inodes_input[cpt] = path_to_last_directory(parsedInput[i],*current_inode,disk);
				} else {
					inodes_input[cpt] = path_to_inode(parsedInput[i],*current_inode,disk);
				}
				cpt++;
			}
			i++;
		}
		
		myln(inodes_input,*current_inode,nb_arg,disk);
		
		free(inodes_input);
		free_input(input,parsedInput);
        return 1;
    
    } else if (strcmp(input, "help") == 0){
        printf("Available commands: ls, touch, cp, mv, cd, rm, rmdir, cat, echo, chmod, df, exit\n");
        printf("In development commands: ln\n");
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
	Inode* parent_inode_create = NULL;
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
			parent_inode_create = inode;
			next_inode = inode_create;
			create = 1;
		} else if(next_inode == NULL && create == 1) {
			printf("Error: Destination path doesn't exist \n");
			remove_tab_index(inode_create,parent_inode_create,disk);
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
	Inode* parent_inode_create = NULL;
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
			mymkdir(file_name,disk,inode);
			inode_create = get_last_inode(*disk);
			parent_inode_create = inode;
			next_inode = inode_create;
			create = 1;
		} else if(next_inode == NULL && create == 1) {
			printf("Error: Path doesn't exist \n");
			remove_tab_index(inode_create,parent_inode_create,disk);
			free_inode(disk,inode_create);
			return NULL;
		}	
	}
	if(create == 0) {
		printf("Error: The file %s already exist in this directory \n",next_inode->name);
	}
	return next_inode;
}

Inode* path_to_last_directory(char* parsedInput,Inode* current_inode,Disk* disk){	
	char* file_name;
	Inode* inode = NULL;
	Inode* next_inode = NULL;
	int not_found = 0;
	
	if(parsedInput[0] == '/') {
		next_inode = disk->inodes;
	} else {
		next_inode = current_inode;
	}
	
	for(file_name = strtok(parsedInput,"/");file_name != NULL;file_name = strtok(NULL,"/")){
		inode = next_inode;
		next_inode = search_file_in_directory(file_name,inode->dir_blocks);
		if(next_inode == NULL && not_found == 0) {
			not_found = 1;
		} else if(next_inode == NULL && not_found == 1) {
			printf("Error: Path doesn't exist \n");;
			return NULL;
		}	
	}
	if(next_inode == NULL) {
		return inode;
	} else {
		return next_inode;
	}
}

Inode** path_to_inode_and_parent_inode(char* parsedInput,Inode* current_inode,Disk* disk){	
	char* file_name;
	Inode* inode = NULL;
	Inode* next_inode = NULL;
	Inode** res = NULL;
	int not_found = 0;
	
	res = (Inode**) malloc(2*sizeof(Inode*));
	
	if(parsedInput[0] == '/') {
		inode = disk->inodes;
		next_inode = disk->inodes;
	} else {
		inode = current_inode;
		next_inode = current_inode;
	}
	
	for(file_name = strtok(parsedInput,"/");file_name != NULL;file_name = strtok(NULL,"/")){
		inode = next_inode;
		if(inode != NULL) {
			next_inode = search_file_in_directory(file_name,inode->dir_blocks);
		}
		if(next_inode == NULL && not_found == 0) {
			not_found = 1;
		} else if(next_inode == NULL && not_found == 1) {
			printf("Error: Path doesn't exist \n");;
			res[0] = NULL;
			res[1] = NULL;
			return res;
		}	
	}
	
	res[0] = next_inode;
	res[1] = inode;
	
	return res;
}

Inode** path_to_destination_and_parent(char* parsedInput,Inode* current_inode,Disk* disk){	
	char* file_name;
	Inode* inode = NULL;
	Inode* next_inode = NULL;
	Inode* inode_create = NULL;
	Inode* parent_inode_create = NULL;
	Inode** res = NULL;
	
	int create = 0;
	res = (Inode**) malloc(2*sizeof(Inode*));
	
	if(parsedInput[0] == '/') {
		inode = disk->inodes;
		next_inode = disk->inodes;
	} else {
		inode = current_inode;
		next_inode = current_inode;
	}
	
	for(file_name = strtok(parsedInput,"/");file_name != NULL;file_name = strtok(NULL,"/")){
		inode = next_inode;
		next_inode = search_file_in_directory(file_name,inode->dir_blocks);
		if(next_inode == NULL && create == 0) {
			mycreate(file_name,disk,inode);
			inode_create = get_last_inode(*disk);
			parent_inode_create = inode;
			next_inode = inode_create;
			create = 1;
		} else if(next_inode == NULL && create == 1) {
			printf("Error: Destination path doesn't exist \n");
			remove_tab_index(inode_create,parent_inode_create,disk);
			free_inode(disk,inode_create);
			res[0] = NULL;
			res[1] = NULL;
			return res;
		}	
	}
	
	res[0] = next_inode;
	res[1] = inode;
	
	return res;
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
