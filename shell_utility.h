#ifndef SHELL_UTILITY
#define SHELL_UTILITY

#include "constants.h"
#include "struct_SGF.h"

char* readline();
char** parse(char* input);
int executeLine(Disk* disk, char* input,Inode* current_inode);
//return the inode associated with the given path, NULL if the path doesn't exist
Inode* path_to_inode(char* parsedInput,Inode* current_inode,Disk* disk); 
//return the inode associated with the given path 
//or create a file and return its inode if the last file doesn't exist 
//or return NULL if a file in the middle of the path doesn't exist
Inode* path_to_destination(char* parsedInput,Inode* current_inode,Disk* disk);
//count the number of path in parsedInput
int count_path(char** parsedInput);
//free the input
void free_input(char* input,char** parsedInput);
//converts the rights input into an array representing the rights
char* convertRights(int rights, int length, char permissions[9]);

#endif
