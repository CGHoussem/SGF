#include <stdio.h>
#include <stdlib.h>
#include "struct_SGF.h"
#include "primitives.h"
#include "command_shell.h"
#include "save_load_SGF.h"
#include "constants.h"

int main(int argc, char** argv){
	Disk disk;
	
	format_disk(&disk);
	printf("Disk root name is: %s\n", disk.inodes->name);
	
	if (save_disk(disk) == 0) {
		printf("Saving of the disk has failed!\n");
	} else {
		printf("The disk has been saved successfully!\n");
	}
	
	if (load_disk(&disk) == 0){
		printf("Loading of the disk has failed!\n");
		free_disk(&disk);
		return EXIT_FAILURE;
	} else {
		printf("The disk has been loaded successfully!\n");
	}

	printf("%s \n",disk.inodes->dir_blocks->tab_index[0].name);
	printf("%s \n",disk.inodes->dir_blocks->tab_index[1].name);

	mkdir("myfiles", &disk, disk.inodes);

	printf("%s \n",disk.inodes->dir_blocks->tab_index[0].name);
	printf("%s \n",disk.inodes->dir_blocks->tab_index[1].name);
	printf("%s \n",disk.inodes->dir_blocks->tab_index[2].name);

	free_disk(&disk);

    return EXIT_SUCCESS;
}
