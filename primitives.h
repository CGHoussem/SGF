#ifndef PRIMITIVE
#define PRIMITIVE

void mymkdir(char* name,Disk* disk,Inode* current_inode); //create a directory
void mycreate(char* name,Disk* disk,Inode* current_inode);//create a file
void myls(Inode* current_inode,char* name_index);//shows the information of a file
void mycp(Inode** inodes,Inode* parent_dest,int number,Disk* disk); //copy the (number - 1) first inodes into the last inode
void mycd (Inode *inode,Inode **current_inode);
void mymv(Inode** inodes,Inode** parent_inode,int number,Disk* disk);//move the (number - 1) first inodes into the last inode
void myrm(Inode* inode,Inode* parent_inode,Disk* disk);//delete inode
void mychmod(Inode** inodes,int number,char permissions[9],Disk* disk);
void mydf(Disk* disk);
void mywrite(Inode* inode,char* output,Disk* disk); // writes the ouput given in the data block(s) of the inode given
char* myread(Inode* inode, char* output); // reads the content of a file
void myln(Inode** inodes,Inode* current_inode,int nb_arg, Disk* disk,char name_link[MAX_FILE_NAME]);

#endif
