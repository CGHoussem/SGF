#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct_SGF.h"
#include "primitives.h"
#include "command_shell.h"
#include "save_load.h"
#include "utility.h"
#include "constants.h"

/** List of commands [Error? | Memory Leak?]
 * 1. ls 		[None | None]
 * 1. ls > f	[NotImplemented]
 * 2. touch 	[None | None]
 * 3. cp		[None| None]
 * 4. mv		[None | None]
 * 5. cd		[None | None]
 * 6. rm		[None | None]
 * 7. rmdir		[None | None]
 * 8. cat		[None | None]
 * 9. echo		[None | None]
 * 9. echo > f	[None | None]
 * 9. echo >> f	[NotImplemented]
 * 10. chmod	[None | None]
 * 11. df		[None | None]
 * 12. ln		[None | None]
 * 13. clear	[None | None]
 * 14. man		[None | None]
 * 15. exit		[None | None]
 * 16. mkdir 	[None [ None]
 **/

/** To implement
 * ls > f
 * echo >> f
 **/ 

int main(int argc, char** argv){
	int running = 1;
	Disk* disk = malloc(sizeof(Disk));
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
