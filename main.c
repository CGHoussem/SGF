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
    char* name = NULL;
	Disk disk;
	Inode* current_inode;
	
	// Formatting the disk
	format_disk(&disk);

    name=disk.inodes->name;
	current_inode = disk.inodes;
	
	// Loading the disk
	if (disk_exists() && load_disk(&disk) == 0){
		printf("Loading of the disk has failed!\n");
	}
	
	// Line-Command Interpreter
	while (running){
        printf("$FMSshell:~%s>", name);
        char* input = readline();
        running = executeLine(&disk, input, current_inode);
    }

	// Saving the disk
	if (save_disk(disk) == 0) {
		printf("Saving of the disk has failed!\n");
	}
	
	free_disk(&disk);

    return EXIT_SUCCESS;
}
