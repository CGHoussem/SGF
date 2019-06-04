#ifndef SHELL_UTILITY
#define SHELL_UTILITY

#include "constants.h"
#include "struct_SGF.h"

char* readline();
char** parse(char* input);
int executeLine(Disk* disk, char* input,Inode* current_inode);
//return a tab of inodes associated with the given files path, NULL if the path doesn't exist
Inode* path_to_inode(char* parsedInput,Inode* current_inode,Disk* disk); 
Inode* path_to_destination(char* parsedInput,Inode* current_inode,Disk* disk);
int count_path(char** parsedInput);

#endif
