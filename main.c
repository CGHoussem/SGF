#include <stdio.h>
#include <stdlib.h>
#include "struct_SGF.h"
#include "primitives.h"
#include "command_shell.h"
#include "save_load_SGF.h"
#include "constants.h"

int main(int argc, char** argv){
	Disk disk;
	Inode inode_test;
	
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

	//free_block_directory(&disk, disk.dir_blocks);

	printf("%s \n",disk.dir_blocks->tab_index[0].name);
	printf("%s \n",disk.dir_blocks->tab_index[1].name);
	
	inode_test=mkdir("test",disk.inodes);
	inode_test.dir_blocks->prev_block=disk.dir_blocks;
	free_inode(&disk,&inode_test);
	
	/*free(disk.dir_blocks->tab_index);
	free(disk.dir_blocks);*/

    return EXIT_SUCCESS;
}
