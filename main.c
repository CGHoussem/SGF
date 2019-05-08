#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct_SGF.h"
#include "primitives.h"
#include "command_shell.h"
#include "save_load_SGF.h"
#include "constants.h"

int main(int argc, char** argv){
	Disk disk;
	
	format_disk(&disk);
	
	if (save_disk(disk) == 0) {
		printf("Saving of the disk has failed!\n");
	} else {
		printf("The disk has been saved successfully!\n");
	}
	
	if (load_disk(&disk) == 0){
		printf("Loading of the disk has failed!\n");
		free(disk.dir_blocks->tab_index);
		free(disk.dir_blocks);
		return EXIT_FAILURE;
	} else {
		printf("The disk has been loaded successfully!\n");
	}
	
	printf("%s \n",disk.dir_blocks->tab_index[0].name);
	printf("%s \n",disk.dir_blocks->tab_index[1].name);

	char cmd[100] = "";
	int index_courant = 0;
    
	while(strcmp(cmd, "exit")) {
		printDir();
		scanf("%s", cmd);
		mycreate(&disk, index_courant, "test.txt"); // creation d'un txt
		ls(&disk, index_courant); // effectue un ls peu importe la cmd
	}
	
	free_disk(&disk);

    return EXIT_SUCCESS;
}
