#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct_SGF.h"
#include "primitives.h"
#include "command_shell.h"
#include "save_load_SGF.h"
#include "constants.h"

int main(int argc, char** argv){
	int running = 1;
	Disk disk;
	Inode* current_inode = NULL;
	
	
	format_disk(&disk);
	printf("Disk root name is: %s\n", disk.inodes->name);
    current_inode = disk.inodes;
		
	/*if (load_disk(&disk) == 0){
		printf("Loading of the disk has failed!\n");
		free_disk(&disk);
		exit(EXIT_FAILURE);
	} else {
		printf("The disk has been loaded successfully!\n");
	}*/
		
	// Line-Command Interpreter
	while (running){
        printf("$FMSshell:~%s>", current_inode->name);
        char* input = readline();
        running = executeLine(&disk, input, &current_inode);
    }

	/*if (save_disk(disk) == 0) {
		printf("Saving of the disk has failed!\n");
	} else {
		printf("The disk has been saved successfully!\n");
	}*/
	
	free_disk(&disk);

    return EXIT_SUCCESS;
}
