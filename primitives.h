#ifndef PRIMITIVE
#define PRIMITIVE

void mymkdir(char* name,Disk* disk,Inode* current_inode); // creates a directory
Inode* mycreate(char* name,Disk* disk,Inode* current_inode); // creates a file and returns its corresponding Inode
void myls(Inode* current_inode,char* name_index);  // shows the information of a file
void mycp(Inode** inodes,Inode* parent_dest,int number,Disk* disk); //copy the (number - 1) first inodes into the last inode
void mycp2(Inode* source_file, Inode* destination_file); //copy the content of the source file into the destination file
void mycd (Inode *inode,Inode **current_inode); // changes the current inode
void mymv(Inode** inodes,Inode** parent_inode,int number,Disk* disk); // move the (number - 1) first inode into the last inode
void myrm(Inode* inode,Inode* parent_inode,Disk* disk); // deletes inode
void mychmod(Inode** inodes,int number,char permissions[10],Disk* disk); // changes the permissions of a file
void mydf(Disk* disk); // shows informations about the virtual disk
void mywrite(Inode* inode,char* output,Disk* disk); // writes the ouput given in the data block(s) of the inode given
char* myread(Inode* inode); // returns the content of a file
void myln(Inode** inodes,Inode* current_inode,int nb_arg, Disk* disk,char name_link[MAX_FILE_NAME]); // creates a symbolic link

#endif
