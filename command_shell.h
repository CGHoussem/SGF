#ifndef COMMAND_SHELL
#define COMMAND_SHELL

//prototypes

void printDir();
Inode mycreate(char* name,Inode prev_inode);
void cp(Inode source, Inode cible);
void mv(Inode source, Inode cible);
void rm(char* name, Inode prev_inode);

#endif
