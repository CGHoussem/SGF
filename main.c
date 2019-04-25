#include <stdio.h>
#include <stdlib.h>
#include "struct_SGF.h"
#include "primitive.h"
#include "command_shell.h"
#include "save_load_SGF.h"

int main(int argc, char** argv){
	Disk disk;
	
	format_disk(&disk);
	printf("%s \n",disk.blocks->b_directory->tab_index[0].name);
	printf("%s \n",disk.blocks->b_directory->tab_index[1].name);
	
	free(disk.blocks->b_directory->tab_index);
	free(disk.blocks->b_directory);
    

    return EXIT_SUCCESS;
}
