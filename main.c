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

	char cmd[100] = "";
	    
	mkdir("myfiles", &disk, disk.inodes);
	mycreate("myfile.txt", &disk, disk.inodes);
	
	printf("%s -> typefichier = %d\n",disk.inodes->dir_blocks->tab_index[0].name, disk.inodes->dir_blocks->tab_index[0].inode->type);
	printf("%s -> typefichier = %d\n",disk.inodes->dir_blocks->tab_index[1].name, disk.inodes->dir_blocks->tab_index[1].inode->type);
	printf("%s -> typefichier = %d\n",disk.inodes->dir_blocks->tab_index[2].name, disk.inodes->dir_blocks->tab_index[2].inode->type);
	printf("%s -> typefichier = %d\n",disk.inodes->dir_blocks->tab_index[3].name, disk.inodes->dir_blocks->tab_index[3].inode->type);

	/*
	while(strcmp(cmd, "exit")) { //mainloop
		printDir();
		scanf("%s", cmd);
		mycreate("myfile.txt", &disk); // creation d'un fichier
		ls(&disk, disk.inodes); // effectue un ls peu importe la cmd
	}*/
	
	free_disk(&disk);

    return EXIT_SUCCESS;
}
