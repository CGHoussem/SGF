#ifndef COMMAND_SHELL
#define COMMAND_SHELL

#include <stdbool.h>
#include "constants.h"
#include "struct_SGF.h"

char* readline();
char** parse(char* input);
// trims the string from the left
char* ltrim(char* untrimmed_str);
int executeLine(Disk* disk, char* input,Inode** current_inode);
//return the inode associated with the given path, NULL if the path doesn't exist
Inode* path_to_inode(char* parsedInput,Inode* current_inode,Disk* disk); 
//return the inode associated with the given path 
//or create a file and return its inode if the last file doesn't exist 
//or return NULL if a directory in the middle of the path doesn't exist
Inode* path_to_destination(char* parsedInput,Inode* current_inode,Disk* disk);
//return the inode associated with the given path 
//or create a directory and return its inode if the last directory doesn't exist 
//or return NULL if a directory in the middle of the path doesn't exist
Inode* path_to_destination_directory(char* parsedInput,Inode* current_inode,Disk* disk);
//return the inode associated with the last directory of the given path, NULL if the path is wrong, save name_link with the name of the last file
Inode* path_to_last_directory(char* parsedInput,Inode* current_inode,Disk* disk,char name_link[MAX_FILE_NAME]);
//return a tab of 2 inodes, the first is the inode associated with the given path, the other one is the parent inode
Inode** path_to_inode_and_parent_inode(char* parsedInput,Inode* current_inode,Disk* disk);
//return a tab of 2 inodes, the first is the inode associated with the given path, 
//(or create a file if the last file doesn't exist),the other one is the parent inode
Inode** path_to_destination_and_parent(char* parsedInput,Inode* current_inode,Disk* disk);
//count the number of path in parsedInput
int count_path(char** parsedInput);
//free the input
void free_input(char* input,char** parsedInput);
//converts the rights input into an array representing the rights
char* convertRights(char* rights, int length, char permissions[10]);
// checks if the disk is filled or not
bool isDiskFilled(Disk* disk, int size);
// returns the number of arguments passed
int nb_arguments(char** parsedInput);
// prints out the manual page of a specific command
int print_manual(char* command);
// commands handling functions
int handleCd(char* input, char** parsedInput,Inode** current_inode,Inode* inode,Disk* disk);
int handleLn(char* input, char** parsedInput,Inode** current_inode,Inode* inode,Inode** inodes_input,Disk* disk);
int handleDf(char* input, char** parsedInput,Disk* disk);
int handleChmod(char* input, char** parsedInput,Inode** current_inode,Inode** inodes_input,Disk* disk);
int handleEcho(char* input, char** parsedInput,Inode** current_inode,Inode* inode,Disk* disk);
int handleCat(char* input, char** parsedInput,Inode** current_inode,Inode* inode,Disk* disk);
int handleRmdir(char* input, char** parsedInput,Inode** current_inode,Inode** inode_and_parent,Disk* disk);
int handleRm(char* input, char** parsedInput,Inode** current_inode,Inode** inode_and_parent,Disk* disk);
int handleMv(char* input, char** parsedInput,Inode** current_inode,Inode** inode_and_parent,Inode** inodes_input,Disk* disk);
int handleCp(char* input, char** parsedInput,Inode** current_inode,Inode** inode_and_parent,Inode** inodes_input,Disk* disk);
int handleTouch(char* input, char** parsedInput,Inode** current_inode,Inode* inode,Disk* disk);
int handleLs(char* input, char** parsedInput,Inode** current_inode,Inode* inode,Disk* disk);
int handleMkdir(char* input, char** parsedInput,Inode** current_inode,Disk* disk);
int handleMan(char* input, char** parsedInput);
#endif
