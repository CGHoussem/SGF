#include <stdio.h>
#include <stdlib.h>
#include "struct_SGF.h"
#include "primitives.h"
#include "command_shell.h"
#include "save_load_SGF.h"

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
		free(disk.blocks->b_directory->tab_index);
		free(disk.blocks->b_directory);
		return EXIT_FAILURE;
	} else {
		printf("The disk has been loaded successfully!\n");
	}

	printf("%s \n",disk.blocks->b_directory->tab_index[0].name);
	printf("%s \n",disk.blocks->b_directory->tab_index[1].name);
	
	free(disk.blocks->b_directory->tab_index);
	free(disk.blocks->b_directory);

    return EXIT_SUCCESS;
}
