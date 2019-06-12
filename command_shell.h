#ifndef COMMAND_SHELL
#define COMMAND_SHELL

#include "constants.h"
#include "struct_SGF.h"

char* readline();
char** parse(char* input);
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
//return the inode associated with the last directory of the given path, NULL if the path is wrong
Inode* path_to_last_directory(char* parsedInput,Inode* current_inode,Disk* disk);
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
char* convertRights(char* rights, int length, char permissions[9]);

#endif
