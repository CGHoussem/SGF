#ifndef PRIMITIVE
#define PRIMITIVE

void mkdir(char* name,Disk* disk,Inode* current_inode); //create a directory
void mycreate(char* name,Disk* disk,Inode* current_inode);//create a file
void ls(Inode* current_inode);//shows all the files in the current directory
void cp(Inode** inodes,int number,Disk* disk); //copy the (number - 1) first inodes into the last inode
void cd (char *name,Inode *current_inode,Disk* disk);
void myrmdir (char *name, Inode *current_inode, Disk* disk);
void mv(Inode source, Inode cible);
void rm(char* name, Inode prev_inode);

#endif
