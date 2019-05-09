#ifndef PRIMITIVE
#define PRIMITIVE

void mkdir(char* name,Disk* disk,Inode* current_inode); //create a directory
void mycreate(char* name,Disk* disk,Inode* current_inode);//create a file
void ls(Disk* disk, int index);
void cp(Inode source, Inode cible);
void mv(Inode source, Inode cible);
void rm(char* name, Inode prev_inode);

#endif
