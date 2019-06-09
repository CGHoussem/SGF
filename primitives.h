#ifndef PRIMITIVE
#define PRIMITIVE

void mkdir(char* name,Disk* disk,Inode* current_inode); //create a directory
void mycreate(char* name,Disk* disk,Inode* current_inode);//create a file
void ls(Inode* current_inode);//shows all the files in the current directory
void cp(Inode** inodes,int number,Disk* disk); //copy the (number - 1) first inodes into the last inode
void cd(char *name,Inode *current_inode,Disk* disk);
void myrmdir(Inode** inodes,int number,Disk* disk);
void mymv(Inode** inodes,int number,Disk* disk);
void myrm(Inode* inode,Disk* disk);
void mychmod(Inode** inodes,int number,char permissions[9],Disk* disk);
void df(Disk* disk);
void mywrite(Inode* inode,char output[BUFFER_SIZE],Disk* disk); // writes the ouput given in the data block(s) of the inode given
void myread(Inode* inode); // reads the content of a file

#endif
