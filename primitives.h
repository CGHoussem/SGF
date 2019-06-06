#ifndef PRIMITIVE
#define PRIMITIVE

void mkdir(char* name,Disk* disk,Inode* current_inode); //create a directory
void mycreate(char* name,Disk* disk,Inode* current_inode);//create a file
void ls(Inode* current_inode);//shows all the files in the current directory
void cp(Inode** inodes,int number,Disk* disk); //copy the (number - 1) first inodes into the last inode
void cd(char *name,Inode *current_inode,Disk* disk);
void rmdir(Inode** inodes,int number,Disk* disk);
void mv(Inode** inodes,int number,Disk* disk);
void rm(Inode** inodes,int number,Disk* disk);
void chmod(Inode** inodes,int number,char permissions[9],Disk* disk);

#endif
