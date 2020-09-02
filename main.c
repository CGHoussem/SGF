#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct_SGF.h"
#include "primitives.h"
#include "command_shell.h"
#include "save_load.h"
#include "utility.h"
#include "constants.h"

int main(int argc, char** argv){
	int running = 1;
	Disk* disk = (Disk*) malloc(sizeof(Disk));
	disk->inodes = NULL;

	system("clear");
	load_disk(disk);
	if (disk->inodes == NULL)
		format_disk(disk);

	Inode* current_inode = disk->inodes;
		
	// Line-Command Interpreter
	while (running){
        printf("$FMSshell:~%s>", current_inode->name);
        char* input = readline();
        running = executeLine(disk, input, &current_inode);
    }

	save_disk(disk);
	free_disk(disk);
    
	return EXIT_SUCCESS;
}
