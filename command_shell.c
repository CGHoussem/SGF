#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "command_shell.h"
#include "primitives.h"
#include "utility.h"

char* readline(){
    char* input = malloc(sizeof(char) * SHELL_BUFFER_SIZE);
    int pos = 0;
    char c;

    while ((c = getchar()) != '\n' && c != EOF){
        input[pos++] = c;
    }
    input[pos] = '\0';

	input = ltrim(input, NULL);

    return input;
}

char* ltrim(char* str, const char* seps){

	size_t totrim;
    if (seps == NULL) {
        seps = "\t\n\v\f\r ";
    }
    totrim = strspn(str, seps);
    if (totrim > 0) {
        size_t len = strlen(str);
        if (totrim == len) {
            str[0] = '\0';
        }
        else {
            memmove(str, str + totrim, len + 1 - totrim);
        }
    }
    return str;
}

char** parse(char* input){
    int i = 0;
    char** args = malloc(sizeof(char*) * SHELL_BUFFER_SIZE);
    // const char* space = " ";
    char* arg = strtok(input, " ");
    while (arg != NULL){
        args[i++] = arg;
        arg = strtok(NULL, " ");
    }
    args[i] = NULL;
    return args;
}

int executeLine(Disk* disk, char* input,Inode** current_inode){
	Inode** inodes_input = NULL;
	Inode* inode = NULL;
	Inode** inode_and_parent = NULL;
    char** parsedInput = parse(input);

	// if the user pressed on the RETURN button without typing any input
	if (strlen(input) == 0){
		return 1;
	}
    if (strcmp(input, "mkdir") == 0){
		return handleMkdir(input, parsedInput, current_inode, disk);
    } else if (strcmp(input, "ls") == 0){
		return handleLs(input, parsedInput, current_inode, inode, disk);
    } else if (strcmp(input, "touch") == 0){
		return handleTouch(input, parsedInput, current_inode, inode, disk);
    } else if (strcmp(input, "cp") == 0){
		return handleCp(input, parsedInput, current_inode, inode_and_parent, inodes_input, disk);
    } else if (strcmp(input, "mv") == 0){
		return handleMv(input, parsedInput, current_inode, inode_and_parent, inodes_input, disk);
    } else if (strcmp(input, "cd") == 0){
		return handleCd(input, parsedInput, current_inode, inode, disk);
    } else if (strcmp(input, "rm") ==0){
		return handleRm(input, parsedInput, current_inode, inode_and_parent, disk);
    } else if (strcmp(input, "rmdir") == 0){
		return handleRmdir(input, parsedInput, current_inode, inode_and_parent, disk);
	} else if (strcmp(input, "cat") == 0){
        return handleCat(input, parsedInput, current_inode, inode, disk);
	} else if (strcmp(input, "echo") == 0){
        return handleEcho(input, parsedInput, current_inode, inode, disk);
	} else if (strcmp(input, "chmod") == 0){
        return handleChmod(input, parsedInput, current_inode, inodes_input, disk);
    } else if (strcmp(input, "df") == 0){
		return handleDf(input, parsedInput,disk);
	} else if (strcmp(input, "ln") == 0){
		return handleLn(input, parsedInput, current_inode, inode, inodes_input, disk);
    } else if (strcmp(input, "man") == 0){
		return handleMan(input, parsedInput);
	} else if (strcmp(input, "help") == 0){
        printf(BOLDWHITE"Available commands:"RESET" ls, touch, cp, mv, cd, rm, rmdir, cat, echo, chmod, df, ln, clear, man, exit\n");
        free_input(input, parsedInput);
        return 1;
    } else if (strcmp(input, "exit") == 0){
		free_input(input, parsedInput);
        return 0;
	} else if(strcmp(input, "clear") == 0) {
		system("clear");
		free_input(input, parsedInput);
		return 1;
    } else {
		print_error("%s: command not found", input);
        free_input(input, parsedInput);
        return 1;
    }
}

char* convertRights(char* rights, int length, char permissions[10]) {

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

	permissions[9] = '\0';

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
		inode = search_file_in_directory(file_name,inode->dirblock);
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
		next_inode = search_file_in_directory(file_name,inode->dirblock);
		if(next_inode == NULL && create == 0) {
			mycreate(file_name,disk,inode);
			inode_create = get_last_inode(*disk);
			parent_inode_create = inode;
			next_inode = inode_create;
			create = 1;
		} else if(next_inode == NULL && create == 1) {
			print_error("Error: Destination path doesn't exist");
			remove_tab_index(inode_create,parent_inode_create,disk);
			free_inode(inode_create);
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
		next_inode = search_file_in_directory(file_name,inode->dirblock);
		if(next_inode == NULL && create == 0) {
			mymkdir(file_name,disk,inode);
			inode_create = get_last_inode(*disk);
			parent_inode_create = inode;
			next_inode = inode_create;
			create = 1;
		} else if(next_inode == NULL && create == 1) {
			print_error("Error: Path doesn't exist");
			remove_tab_index(inode_create,parent_inode_create,disk);
			free_inode(inode_create);
			return NULL;
		}
	}
	if(create == 0) {
		print_error("Error: The file %s already exist in this directory",next_inode->name);
	}
	return next_inode;
}

Inode* path_to_last_directory(char* parsedInput,Inode* current_inode,Disk* disk,char name_link[MAX_FILE_NAME]){
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
		strcpy(name_link,file_name);
		inode = next_inode;
		next_inode = search_file_in_directory(file_name,inode->dirblock);
		if(next_inode == NULL && not_found == 0) {
			not_found = 1;
		} else if(next_inode == NULL && not_found == 1) {
			print_error("Error: Path doesn't exist");
			return NULL;
		}
	}
	if(next_inode == NULL) {
		return inode;
	} else {
		strcpy(name_link,"\0");
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
			next_inode = search_file_in_directory(file_name,inode->dirblock);
		}
		if(next_inode == NULL && not_found == 0) {
			not_found = 1;
		} else if(next_inode == NULL && not_found == 1) {
			print_error("ERROR: Path doesn't exist");
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
		next_inode = search_file_in_directory(file_name,inode->dirblock);
		if(next_inode == NULL && create == 0) {
			mycreate(file_name,disk,inode);
			inode_create = get_last_inode(*disk);
			parent_inode_create = inode;
			next_inode = inode_create;
			create = 1;
		} else if(next_inode == NULL && create == 1) {
			print_error("ERROR: Destination path doesn't exist");
			remove_tab_index(inode_create,parent_inode_create,disk);
			free_inode(inode_create);
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

void free_input(char* input, char** parsedInput) {
	free(input);
	free(parsedInput);
}

bool isDiskFilled(Disk* disk, int size) {
	return ((DISK_BYTES_LIMIT-(count_disk_datablocks(*disk)*BUFFER_SIZE)-size) >= BUFFER_SIZE);
}

int handleCd(char* input, char** parsedInput,Inode** current_inode,Inode* inode,Disk* disk) {
	if(parsedInput[1] != NULL){
		inode = path_to_inode(parsedInput[1],*current_inode,disk);
		if(inode != NULL && inode->type == DIRECTORY) {
			mycd(inode, current_inode);
		} else {
			print_error("Error: directory doesn't exist");
		}
	} else {
		print_error("Missing input");
	}
	free_input(input, parsedInput);
	return 1;
}

int handleLn(char* input, char** parsedInput,Inode** current_inode,Inode* inode,Inode** inodes_input,Disk* disk) {
	int nb_arg,i,cpt;
	char name_link[MAX_FILE_NAME];

	nb_arg = count_path(parsedInput);
	if(nb_arg < 1) {
		print_error("Error: Missing file input");
		free_input(input, parsedInput);
		return 1;
	}

	inodes_input = (Inode**) malloc(nb_arg * sizeof(Inode*));

	i = 1;
	cpt = 0;
	while(parsedInput[i] != NULL) {
		if(parsedInput[i][0] != '-') {
			if(nb_arg > 1 && cpt == nb_arg-1) {
				inodes_input[cpt] = path_to_last_directory(parsedInput[i],*current_inode,disk,name_link);
			} else {
				inodes_input[cpt] = path_to_inode(parsedInput[i],*current_inode,disk);
			}
			cpt++;
		}
		i++;
	}

	myln(inodes_input,*current_inode,nb_arg,disk,name_link);

	free(inodes_input);
	free_input(input, parsedInput);
	return 1;
}

int handleDf(char* input, char** parsedInput,Disk* disk) {
	int nb_arg = count_path(parsedInput);

	if(nb_arg > 0) {
		print_error("df doesn't require arguments");
		free_input(input, parsedInput);
		return 1;
	}

	mydf(disk);

	return 1;
}

int handleChmod(char* input, char** parsedInput,Inode** current_inode,Inode** inodes_input,Disk* disk) {
	int i,cpt,nb_arg = count_path(parsedInput);

	if(nb_arg < 2) {
		print_error("Missing input(s)");
		free_input(input, parsedInput);
		return 1;
	}

	int length = strlen(parsedInput[1]);

	// verifying that the given input is in the right range
	if(length < 1 || length > 3) {
		print_error("Rights input is incorrect");
		free_input(input, parsedInput);
		return 1;
	}

	int digit;

	// verifying the integer input concerning the rights
	for(int i=0;i<length;i++) {
		digit = (int)parsedInput[1][i] - ASCII_OFFSET;
		if(isdigit(parsedInput[1][i]))
		{
			if(digit == 8 || digit == 9) {
				print_error("Rights input is incorrect");
				free_input(input, parsedInput);
				return 1;
			}
		}
		else {
			print_error("Rights input is incorrect");
			free_input(input, parsedInput);
			return 1;
		}
	}

	//files input of chmod
	inodes_input = (Inode**) malloc(nb_arg*sizeof(Inode*));

	i = 2;
	cpt = 0;

	while(parsedInput[i] != NULL) {
		if(parsedInput[i][0] != '-') {
			inodes_input[cpt] = path_to_inode(parsedInput[i],*current_inode,disk);
			if(inodes_input[cpt] == NULL) {
				print_error("Error: argument %d is not an existing file/directory",(cpt+2));
				free_input(input, parsedInput);
				free(inodes_input);
				return 1;
			}
			cpt++;
		}
		i++;
	}

	char permissions[10];
	mychmod(inodes_input,nb_arg-1,convertRights(parsedInput[1], length, permissions),disk);
	free(inodes_input);
	free_input(input, parsedInput);
	return 1;

	return 1;
}

int handleEcho(char* input, char** parsedInput,Inode** current_inode,Inode* inode,Disk* disk) {
	int i,j,nb_arg = count_path(parsedInput);

	if(nb_arg < 1) {
		print_error("Error: missing inputs");
		free_input(input, parsedInput);
		return 1;
	}

	// counting quotes
	int nb_quotes = 0;
	for(i=1;i<=nb_arg;i++)
		for(j=0;j<strlen(parsedInput[i]);j++)
			nb_quotes += (parsedInput[i][j] == '"');

	// checks if quotes are opened but not closed
	if(nb_quotes%2 != 0) {
		print_error("Error: you have entered %d quotes, 1 missing quote", nb_quotes);
		free_input(input, parsedInput);
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
				print_error("Error: missing file input");
				free_input(input, parsedInput);
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
		free_input(input, parsedInput);
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
					print_error("Error: size of the input is too high");
					free_input(input, parsedInput);
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
					print_error("Error: size of the input is too high");
					free_input(input, parsedInput);
					return 1;
				}
			}
			output[strlen(output)] = ' ';
		}
	}
	output[strlen(output)] = '\0';

	inode = path_to_inode(parsedInput[redirectionIndex+1],*current_inode,disk);
	if(inode == NULL) {
		inode = path_to_destination(parsedInput[redirectionIndex+1],*current_inode,disk);
		if(inode == NULL) {
			print_error("Error at argument %d", i);
			free_input(input, parsedInput);
			return 1;

		} else {
			inode->modification_date = time(NULL);
			mywrite(inode,output,disk);
		}
	}
	else if(inode->type != TEXT) {
		print_error("Error: argument %d is not a file", i);
	}
	else {
		mywrite(inode,output,disk);
	}

	free_input(input, parsedInput);
	return 1;
}

int handleCat(char* input, char** parsedInput,Inode** current_inode,Inode* inode,Disk* disk) {
	int i, hasRedirection = 0, redirectionIndex = 0;
	int nb_arg = count_path(parsedInput);

	if(nb_arg < 1) {
		print_error("Error: missing input");
		free_input(input, parsedInput);
		return 1;
	}

	// checks if there is redirection
	for(i=1;i<=nb_arg;i++) {
		if(strcmp(parsedInput[i], ">") == 0)
		{
			// verifying if there's a file after the redirection
			if(i == nb_arg || strcmp(parsedInput[i+1], "cat") == 0) {
				print_error("Error: missing or bad file input");
				free_input(input, parsedInput);
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
			inode = path_to_inode(parsedInput[i],*current_inode,disk);
			if(inode == NULL) {
				print_error("Error: argument %d is not an existing file", i);
			}
			else if(inode->type != TEXT) {
				print_error("Error: argument %d is not a file", i);
			}
			else if (inode->datablocks != NULL){
				char* output = myread(inode);
				printf("%s\n", output);
				free(output);
			}
		}
		free_input(input, parsedInput);
		return 1;
	}

	// int content_inode_size = 0;
	char content_inodes[MAX_REDIRECTION_SIZE] = "";
	content_inodes[0] = '\0';

	// First part of inodes (before the redirection char)
	for(i=1;i<redirectionIndex;i++) {
		inode = path_to_inode(parsedInput[i],*current_inode,disk);
		if(inode == NULL) {
			print_error("ERROR: The file '%s' does not exist!", parsedInput[i]);
			free_input(input, parsedInput);
			return 1;
		}
		else if(inode->type != TEXT) {
			print_error("ERROR: '%s' is not a file!", parsedInput[i]);
			free_input(input, parsedInput);
			return 1;
		}
		else {
			char* content_inode = NULL;
			content_inode = myread(inode);
			strcat(content_inodes, content_inode);
			free(content_inode);
		}
	}

	// Second part of the files which will be displayed (after the file which will be modified)
	if(nb_arg > redirectionIndex+1) {
		for(i=redirectionIndex+2;i<=nb_arg;i++) {
			for(i=1;i<redirectionIndex;i++) {
				inode = path_to_inode(parsedInput[i],*current_inode,disk);
				if(inode == NULL) {
					print_error("ERROR: The file '%s' does not exist!", parsedInput[i]);
					free_input(input, parsedInput);
					return 1;
				}
				else if(inode->type != TEXT) {
					print_error("ERROR: '%s' is not a file!", parsedInput[i]);
					free_input(input, parsedInput);
					return 1;
				}
				else {
					char* content_inode = NULL;
					content_inode = myread(inode);
					strcat(content_inodes, content_inode);
					free(content_inode);
				}
			}
		}
	}

	inode = path_to_inode(parsedInput[redirectionIndex+1],*current_inode,disk);

	inode = path_to_inode(parsedInput[redirectionIndex+1],*current_inode,disk);
	if(inode == NULL) {
		inode = path_to_destination(parsedInput[redirectionIndex+1],*current_inode,disk);
		if(inode == NULL) {
			print_error("Error at argument %d", i);
			free_input(input, parsedInput);
			return 1;

		} else {
			inode->modification_date = time(NULL);
			//mywrite(inode,content_inodes,disk);
		}
	}
	else if(inode->type != TEXT) {
		print_error("ERROR: '%s' is not a file!", inode->name);
	}
	else {
		//mywrite(inode,content_inodes,disk);
	}

	free_input(input, parsedInput);
	return 1;
}

int handleRmdir(char* input, char** parsedInput,Inode** current_inode,Inode** inode_and_parent,Disk* disk) {
	int i, nb_arg = count_path(parsedInput);

	if(nb_arg < 1) {
		print_error("ERROR: Missing directory input!");
		free_input(input, parsedInput);
		return 1;
	}

	i = 1;

	while(parsedInput[i] != NULL) {
		if(parsedInput[i][0] != '-') {
			inode_and_parent = path_to_inode_and_parent_inode(parsedInput[i],*current_inode,disk);
			if(inode_and_parent[0] == NULL) {
				print_error("ERROR: The directory '%s' does not exist!", parsedInput[i]);
			}
			else if(inode_and_parent[0]->type != DIRECTORY) {
				print_error("ERROR: '%s' is not a directory!", parsedInput[i]);
			}
			else if(inode_and_parent[0]->dirblock->nb_index > 2) {
				print_error("ERROR: The directory '%s' is not empty!", parsedInput[i]);
			}
			else if(inode_and_parent[0] == *current_inode) {
				print_error("ERROR: Cannot delete the current directory");
			}
			else {
				myrm(inode_and_parent[0],inode_and_parent[1],disk);
			}
			free(inode_and_parent);
		}
		i++;
	}

	free_input(input, parsedInput);
	return 1;
}

int handleRm(char* input, char** parsedInput,Inode** current_inode,Inode** inode_and_parent,Disk* disk) {
	int i,nb_arg = count_path(parsedInput);

	if(nb_arg < 1) {
		print_error("ERROR: Missing file input!");
		free_input(input, parsedInput);
		return 1;
	}

	i = 1;

	while(parsedInput[i] != NULL) {
		if(parsedInput[i][0] != '-') {
			inode_and_parent = path_to_inode_and_parent_inode(parsedInput[i],*current_inode,disk);
			if(inode_and_parent[0] == NULL) {
				print_error("ERROR: the file '%s' does not exist!", parsedInput[i]);
			}
			else if(inode_and_parent[0]->type == DIRECTORY) {
				print_error("ERROR: '%s' is a directory!", parsedInput[i]);
			}
			else {
				myrm(inode_and_parent[0],inode_and_parent[1],disk);
			}
			free(inode_and_parent);
		}
		i++;
	}

	free_input(input, parsedInput);
	return 1;
}

// TODO: COMPLICATED FOR NO REASON
int handleMv(char* input, char** parsedInput,Inode** current_inode,Inode** inode_and_parent,Inode** inodes_input,Disk* disk) {
	int i,cpt,nb_arg = count_path(parsedInput);
	Inode** parent_inodes_input = NULL;
	char failure = 0;

	if(nb_arg < 2) {
		print_error("ERROR: Missing file input");
		free_input(input, parsedInput);
		return 1;
	}

	inodes_input = (Inode**) malloc(nb_arg*sizeof(Inode*)); //input of move
	parent_inodes_input = (Inode**) malloc(nb_arg*sizeof(Inode*));

	i = 1;
	cpt = 0;

	while(parsedInput[i] != NULL) {
		if(parsedInput[i][0] != '-') { // if it's not a command option
			if(cpt == nb_arg-1) { // destination file/folder
				inode_and_parent = path_to_destination_and_parent(parsedInput[i],*current_inode,disk);
				inodes_input[cpt] = inode_and_parent[0];
				parent_inodes_input[cpt] = inode_and_parent[1];
				if(inodes_input[cpt] == NULL) { //destination file/folder doesn't exist
					print_error("ERROR: Destination file/folder doesn't exist");
					failure = 1;
				}
				if(nb_arg != 2 && (inodes_input[cpt])->type != DIRECTORY) {
					print_error("ERROR: More than a file to move into a file");
					remove_tab_index(inodes_input[cpt],parent_inodes_input[cpt],disk);
					free_inode(inodes_input[cpt]);
					failure = 1;
				}
				if (failure == 1)
				{
					free(inodes_input[0]);
					free(inodes_input[1]);
					free(inodes_input);
					free(parent_inodes_input);
					free(inode_and_parent[0]);
					free(inode_and_parent[1]);
					free(inode_and_parent);
					free_input(input, parsedInput);
					return 1;
				}
			} else { //source file
				inode_and_parent = path_to_inode_and_parent_inode(parsedInput[i],*current_inode,disk);
				inodes_input[cpt] = inode_and_parent[0];
				parent_inodes_input[cpt] = inode_and_parent[1];
				if(inodes_input[cpt] == NULL) {
					print_error("ERROR: the file '%s' doest not exist!", parsedInput[i]);
					failure = 1;
				}else if(inodes_input[cpt]->type == DIRECTORY) {
					print_error("ERROR: '%s' is a directory!", parsedInput[i]);
					failure = 1;
				}
				if (failure == 1)
				{
					free(inodes_input[0]);
					free(inodes_input[1]);
					free(inodes_input);
					free(parent_inodes_input);
					free(inode_and_parent[0]);
					free(inode_and_parent[1]);
					free(inode_and_parent);
					free_input(input, parsedInput);
					return 1;
				}
			}
			cpt++;
			free(inode_and_parent[0]);
			free(inode_and_parent[1]);
			free(inode_and_parent);
		}
		i++;
	}

	#if DEBUG == 1
	for (int i = 0; i < nb_arg; i++) {
		print_debug("inodes_input[%d].name = %s", i, inodes_input[i]->name);
		print_debug("parent_inodes_input[%d].name = %s", i, parent_inodes_input[i]->name);
	}
	#endif


	mymv(inodes_input,parent_inodes_input,nb_arg-1,disk);
	free(inodes_input[0]);
	free(inodes_input[1]);
	free(inodes_input);
	free(parent_inodes_input);
	free_input(input, parsedInput);
	return 1;
}

int handleCp(char* input, char** parsedInput,Inode** current_inode,Inode** inode_and_parent,Inode** inodes_input,Disk* disk) {
	int i,cpt,nb_arg = count_path(parsedInput);

	if(nb_arg < 2) {
		print_error("Missing file input");
		free_input(input, parsedInput);
		return 1;
	}

	inodes_input = (Inode**) malloc(nb_arg*sizeof(Inode*)); //input of cp

	i = 1;
	cpt = 0;

	// Verification and preparation
	while(parsedInput[i] != NULL) {
		if(parsedInput[i][0] != '-') {
			if(cpt == nb_arg-1) { //destination file
				inode_and_parent = path_to_destination_and_parent(parsedInput[i],*current_inode,disk);
				inodes_input[cpt] = inode_and_parent[0];
				if(inodes_input[cpt] == NULL) { //destination file doesn't exist
					free(inodes_input);
					free_input(input, parsedInput);
					return 1;
				}
				if(nb_arg != 2 && (inodes_input[cpt])->type != DIRECTORY) {
					print_error("Error: More than a file to copy into a file");
					remove_tab_index(inodes_input[cpt],inode_and_parent[1],disk);
					free_inode(inodes_input[cpt]);
					free(inodes_input);
					free_input(input, parsedInput);
					return 1;
				}
			} else { //source file
				inodes_input[cpt] = path_to_inode(parsedInput[i],*current_inode,disk);
				if(inodes_input[cpt] == NULL) {
					print_error("ERROR: file '%s' does not exist!", parsedInput[i]);
					free_input(input, parsedInput);
					free(inodes_input);
					return 1;
				}else if(inodes_input[cpt]->type == DIRECTORY) {
					print_error("ERROR: '%s' is a directory!", parsedInput[i]);
					free_input(input, parsedInput);
					free(inodes_input);
					return 1;
				}
			}
			cpt++;
		}
		i++;
	}
	//mycp(inodes_input,inode_and_parent[1],nb_arg,disk);
	free(inode_and_parent);
	free(inodes_input);
	free_input(input, parsedInput);
	return 1;
}

int handleTouch(char* input, char** parsedInput,Inode** current_inode,Inode* inode,Disk* disk) {
	int i,nb_arg = count_path(parsedInput);

	if(nb_arg < 1) {
		print_error("Missing file input");
		free_input(input, parsedInput);
		return 1;
	}

	i = 1;
	while(parsedInput[i] != NULL) {
		if(parsedInput[i][0] != '-') {
			inode = path_to_destination(parsedInput[i],*current_inode,disk);
			if(inode == NULL) {
				print_error("Error at argument %d",i);
			} else {
				inode->modification_date = time(NULL);
			}
		}
		i++;
	}

	free_input(input, parsedInput);
	return 1;

}

int handleLs(char* input, char** parsedInput,Inode** current_inode,Inode* inode,Disk* disk) {
	int i,j,nb_arg = count_path(parsedInput);

	if(nb_arg < 1) {
		for(j=0;j<(*current_inode)->dirblock->nb_index;j++) {
			myls((*current_inode)->dirblock->tab_index[j].inode,(*current_inode)->dirblock->tab_index[j].name);
		}
		free_input(input, parsedInput);
		return 1;
	}

	i = 1;
	while(parsedInput[i] != NULL) {
		if(parsedInput[i][0] != '-') {
			inode = path_to_inode(parsedInput[i],*current_inode,disk);
			if(inode == NULL) {
				print_error("Error at argument %d : path doesn't exist",i);
			} else if(inode->type != DIRECTORY) {
				myls(inode,NULL);
			} else { // informations will be shown
				printf("%s : \n",inode->name);
				for(j=0;j<inode->dirblock->nb_index;j++) {
					myls(inode->dirblock->tab_index[j].inode,inode->dirblock->tab_index[j].name);
				}
			}
		}
		i++;
	}
	free_input(input, parsedInput);
	return 1;
}

int handleMkdir(char* input, char** parsedInput,Inode** current_inode,Disk* disk) {
	int i,nb_arg = count_path(parsedInput);

	if(nb_arg < 1) {
		print_error("Missing file input");
		free_input(input, parsedInput);
		return 1;
	}

	i = 1;
	while(parsedInput[i] != NULL) {
		if(parsedInput[i][0] != '-') {
			// creates a directory
			if(path_to_destination_directory(parsedInput[i],*current_inode,disk) == NULL) {
				print_error("Error at argument %d, the directory was not created", i);
			}
		}
		i++;
	}

	free_input(input, parsedInput);
	return 1;
}

int handleMan(char* input, char** parsedInput){
	int nbr_args = nb_arguments(parsedInput);
	if (nbr_args == 0){
		printf("What manual page do you want?\n");
	} else {
		for (int i = 1; i <= nbr_args; i++) {
			int ret = print_manual(parsedInput[i]);

			if (ret == -1){
				print_error("No manual entry for %s", parsedInput[i]);
				continue;
			}
			printf(HIGHTLIGHT "Press [RETURN] to go to the next command or leave" RESET);

			char c;
			scanf("%c", &c);
			system("clear");
		}
	}

	return 1;
}

int print_manual(char* command){
	if (strcmp(command, "ln") == 0){
		printf("In the 1st form create a link to TARGET with the name LINK_NAME. In the 2nd form, create a link to TARGET in the current directory. In the 3rd and the 4th forms, create links to each TARGET in DIRECTORY. Create hard links by default, symbolic links with " BOLDWHITE "--symbolic" RESET ".");
	} else if (strcmp(command, "df") == 0){
		printf("This manual page documents the GNU version of " BOLDWHITE "df" RESET ". " BOLDWHITE "df" RESET " displays the amount of disk space available on the file system containing each file name argument. If no file name is given, the space available on all currently moounted file systems is shown...\n");
	} else if (strcmp(command, "chmod") == 0){
		printf("This manual page documents the GNU version of " BOLDWHITE "chmod" RESET ". " BOLDWHITE "chmod" RESET " changes the file mode bits of each given file according to mode, which can be either symbolic representation of changes to make, or an octal number representing the bit pattern of the new mode bits..\n");
	} else if (strcmp(command, "echo") == 0){
		printf("Echo the STRING(s) to standard output.\n");
	} else if (strcmp(command, "cat") == 0){
		printf("Concatenante FILE(s) to standard output.\n");
	} else if (strcmp(command, "rmdir") == 0){
		printf("Remove the DIRECTORY(ies), if they are empty.\n");
	} else if (strcmp(command, "rm") == 0){
		printf("This manual page documents the GNU version of " BOLDWHITE "rm" RESET ". " BOLDWHITE "rm" RESET " removes each specified file. By default, it does not remove directories.\n");
	} else if (strcmp(command, "mv") == 0){
		printf("Rename SOURCE to DEST, or move SOURCE(s) to DIRECTORY.\n");
	} else if (strcmp(command, "cp") == 0){
		printf("Copy SOURCE to DEST, or multiple SOURCE(s) to DIRECTORY.\n");
	} else if (strcmp(command, "touch") == 0){
		printf("Update the access and modification times of each FILE to the current time.\n");
	} else if (strcmp(command, "ls") == 0){
		printf("List information about the FILEs (the current directory by default). Sort entries alphabetically if none of -cftuvSUX nor --sort is specified.");
	} else if (strcmp(command, "mkdir") == 0){
		printf("Create the DIRECTORY(ies), if they do not already exist.\n");
	} else if (strcmp(command, "man") == 0){
		printf(BOLDWHITE "man" RESET " is the system's manual pager. Each page argument givent to " BOLDWHITE "man" RESET " is the normally the name of a program, utility or function.\n");
	} else if (strcmp(command, "exit") == 0){
		printf("The exit() function causes normal process termination and the value of status & 0377 is returned to the parent.\n");
	} else
		return -1;
	return 0;
}

int nb_arguments(char** parsedInput){
	int counter = -1;
	while (parsedInput[++counter] != NULL);
	return counter - 1;
}