#ifndef COMMAND_SHELL
#define COMMAND_SHELL

//prototypes

void printDir();
void mycreate(Disk* disk, int index, char* name);
void cp(Inode source, Inode cible);
void mv(Inode source, Inode cible);
void rm(char* name, Inode prev_inode);

#endif
