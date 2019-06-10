#ifndef PRIMITIVE
#define PRIMITIVE

void mkdir(char* name,Disk* disk,Inode* current_inode); //create a directory
void mycreate(char* name,Disk* disk,Inode* current_inode);//create a file
void ls(Inode* current_inode);//shows all the files in the current directory
void cp(Inode** inodes,int number,Disk* disk); //copy the (number - 1) first inodes into the last inode
void cd(char *name,Inode *current_inode,Disk* disk);
void mymv(Inode** inodes,int number,Disk* disk);//move the (number - 1) first inodes into the last inode
void myrm(Inode* inode,Disk* disk);//delete inode
void mychmod(Inode** inodes,int number,char permissions[9],Disk* disk);
void df(Disk* disk);
void mywrite(Inode* inode,char* output,Disk* disk); // writes the ouput given in the data block(s) of the inode given
char* myread(Inode* inode, char* output); // reads the content of a file
//void ln(Inode** inode, Inode* current_inode,Disk* disk);


#endif
