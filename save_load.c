#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "save_load.h"

void save_disk(Disk disk){
    if (disk.inodes != NULL)
	    save_inodes(disk.inodes);
    if (disk.dir_blocks != NULL)
        save_dir_blocks(disk.dir_blocks);
    if (disk.data_blocks != NULL)
        save_data_blocks(disk.data_blocks);
}

void save_inodes(Inode* inodes){
    Inode* temp = (Inode*) malloc(sizeof(Inode));
    temp = inodes;
    while (temp != NULL) {
        save_inode(temp);
        temp = temp->next_inode;
    } 
    free(temp);
}

void save_dir_blocks(Directory_block* dir_blocks){
    Directory_block* temp = (Directory_block*) malloc(sizeof(Directory_block));
    temp = dir_blocks;
    while (temp != NULL){
        save_dir_block(temp);
        temp = temp->next_block;
    }
    free(temp);
}

void save_data_blocks(Data_block* data_blocks){
    Data_block* temp = (Data_block*) malloc(sizeof(Data_block));
    temp = data_blocks;
    while (temp != NULL){
        save_data_block(temp);
        temp = temp->next_block;
    }
    free(temp);
}

void save_inode(Inode* inode){
    // TODO set a constant for the inode saving filename
    char* filename = malloc(sizeof(char) * 9);
    // TODO set a counter for the inodes
    sprintf(filename, "%d.inode", 1);
    FILE* file = fopen(filename, "wb");

    fwrite(inode->name, 1, sizeof(char)*strlen(inode->name), file);
    // TODO finish writing all of the metadata

    free(filename);
    fclose(file);
}

void save_dir_block(Directory_block* dir_block){

}

void save_data_block(Data_block* data_block){

}
