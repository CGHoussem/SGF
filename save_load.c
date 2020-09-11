#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "save_load.h"
#include "utility.h"

void save_disk(Disk* disk){
    JSON_Value* root_value = json_value_init_object();
    JSON_Object* root_object = json_value_get_object(root_value);

    Inode* inode = disk->inodes;
    while (inode != NULL){
        char* inode_label = malloc(sizeof(char) * 128);
        sprintf(inode_label, "inode_%ld", inode->uid);

        JSON_Value* inode_json_value = save_inode(inode);
        
        json_object_set_value(root_object, inode_label, inode_json_value);
        
        free(inode_label);
        inode = inode->next_inode;
    }

    json_serialize_to_file_pretty(root_value, DISK_FILENAME);
    json_value_free(root_value);
}

JSON_Value* save_inode(Inode* inode){
    JSON_Value* root_value = json_value_init_object();
    JSON_Object* root_object = json_value_get_object(root_value);

    json_object_set_number(root_object, "uid", inode->uid);
    json_object_set_string(root_object, "name", inode->name);
    json_object_set_string(root_object, "permissions", inode->permissions);
    json_object_set_number(root_object, "type", inode->type);
    json_object_set_number(root_object, "creation_date", inode->creation_date);
    json_object_set_number(root_object, "modification_date", inode->modification_date);

    if (inode->type <= 2) { // file
        save_datablocks(root_object, inode);
    } else { // directory
        char* buffer = save_dirblock(inode, inode->dirblock);
        json_object_set_value(root_object, "dirblock", json_parse_string(buffer));
        free(buffer);
    }

    return root_value;
}

void save_datablocks(JSON_Object* json_object, Inode* inode){
    char* buffer = malloc(sizeof(char) * 65556);
    strcpy(buffer, "[");
    
    DataBlock* block = inode->datablocks;
    while (block != NULL) {
        strcat(buffer, "\"");
        strcat(buffer, block->data);
        strcat(buffer, "\"");
        
        if (block->next_block != NULL){
            strcat(buffer, ",");
        }
        block = block->next_block;
    }
    strcat(buffer, "]");
    json_object_set_value(json_object, "datablock", json_parse_string(buffer));
    
    free(buffer);
    free(block);
}

char* save_dirblock(Inode* inode, DirectoryBlock* dir_block){
    char* buffer = (char*) malloc(sizeof(char*) * 65556);
    strcpy(buffer, "[");
    
    Index* current_index = dir_block->indexes;

    while (current_index != NULL) {
        JSON_Value* index_json_value = save_index(current_index);

        char* serialized_string = json_serialize_to_string(index_json_value);
        strcat(buffer, serialized_string);
            
        json_free_serialized_string(serialized_string);
        json_value_free(index_json_value);

        current_index = current_index->next_index;
        if (current_index != NULL) 
            strcat(buffer, ",");
    }
    strcat(buffer, "]");
    
    return buffer;
}

JSON_Value* save_index(Index* index){
    JSON_Value* root_value = json_value_init_object();
    JSON_Object* root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "name", index->name);
    json_object_set_number(root_object, "inode_uid", index->inode->uid);

    return root_value;
}

void load_disk(Disk* disk){
    // Verifying if the disk file exists
    FILE* disk_f = fopen(DISK_FILENAME, "r");
    if (disk_f == NULL)
        return;
    fclose(disk_f);

    disk->inodes = NULL;

    JSON_Value* disk_json_value = json_parse_file(DISK_FILENAME);
    JSON_Object* object = json_value_get_object(disk_json_value);
    
    size_t count = json_object_get_count(object);
    // looping over inode json objects
    // PART 1
    for (size_t i = 0; i < count; i++){
        // get the inode from the json inode object
        Inode* inode = load_incomplete_inode(object, i);

        // append the inode to the disk
        disk->inodes = append_inode_to_disk(disk, inode);
    }

    // PART 2
    for (size_t i = 0; i < count; i++){
        // complete (update) the inode
        load_complete_inode(disk, object, i);
    }

    json_value_free(disk_json_value);

    printf(BOLDGREEN"Disk has been successfully loaded!\n"RESET);
}

Inode* append_inode_to_disk(Disk* disk, Inode* inode){
    if (disk->inodes == NULL){
        disk->inodes = inode;
    } else if (inode != NULL) {
        Inode* temp = disk->inodes;

        while (temp->next_inode != NULL){
            temp = temp->next_inode;
        }

        temp->next_inode = inode;
    }
    return disk->inodes;
}

Inode* load_incomplete_inode(JSON_Object* object, size_t index) {
    JSON_Value* inode_json_value = json_object_get_value_at(object, index);
    JSON_Object* inode_json_object = json_value_get_object(inode_json_value);

    Inode* inode = (Inode*) malloc(sizeof(Inode));
    inode->uid = (unsigned long) json_object_get_number(inode_json_object, "uid");
    strcpy(inode->name, json_object_get_string(inode_json_object, "name"));
    strcpy(inode->permissions, json_object_get_string(inode_json_object, "permissions"));
    inode->type = (int)json_object_get_number(inode_json_object, "type");
    inode->creation_date = (time_t)json_object_get_number(inode_json_object, "creation_date");
    inode->modification_date = (time_t)json_object_get_number(inode_json_object, "modification_date");
    inode->next_inode = NULL;
    inode->datablocks = NULL;
    inode->dirblock = NULL;

    if (inode->type <= 2) {
        inode->datablocks = load_datablocks(inode_json_object, inode);
    }

    return inode;
}

void load_complete_inode(Disk* disk, JSON_Object* object, size_t index) {
    JSON_Value* inode_json_value = json_object_get_value_at(object, index);
    JSON_Object* inode_json_object = json_value_get_object(inode_json_value);

    // get the inode from the disk by its uid
    unsigned long inode_uid = (unsigned long) json_object_get_number(inode_json_object, "uid");
    Inode* inode = get_inode_by_uid(disk, inode_uid);

    int type = (int)json_object_get_number(inode_json_object, "type");

    if (type == 3)
        inode->dirblock = load_dirblocks(disk, inode_json_object, inode);
}

DataBlock* load_datablocks(JSON_Object* json_object, Inode* inode) {
    JSON_Array* data_json_array = json_object_get_array(json_object, "datablock");
    size_t block_count = json_array_get_count(data_json_array);
    
    if (block_count == 0) {
        return NULL;
    }

    DataBlock* blocks = NULL;
    for (size_t i = 0; i < block_count; i++) {
        DataBlock* new_block = malloc(sizeof(DataBlock));
        
        strcpy(new_block->data, json_array_get_string(data_json_array, i));
        new_block->next_block = NULL;
        
        blocks = append_datablock_to_list(blocks, new_block);
    }

    return blocks;
}

DirectoryBlock* load_dirblocks(Disk* disk, JSON_Object* object, Inode* inode) {
    DirectoryBlock* block = malloc(sizeof(DirectoryBlock));
    JSON_Array* indexes_json = json_object_get_array(object, "dirblock");
    
    size_t count = json_array_get_count(indexes_json);
    block->indexes = NULL;
    block->next_block = NULL;
    
    for (size_t i = 0; i < count; i++){
        JSON_Object* index_json_object = json_array_get_object(indexes_json, i);
        block->indexes = append_index_to_list(block->indexes, load_index(disk, index_json_object));
    }

    return block;
}

Index* load_index(Disk* disk, JSON_Object* object) {
    Index* index = malloc(sizeof(Index));

    strcpy(index->name, json_object_get_string(object, "name"));
    unsigned long inode_id = (unsigned long) json_object_get_number(object, "inode_uid");
    index->inode = get_inode_by_uid(disk, inode_id);
    index->next_index = NULL;

    return index;
}

Inode* get_inode_by_uid(Disk* disk, unsigned long uid){
    Inode* inode = NULL;
    Inode* temp = disk->inodes;

    while (inode == NULL && temp != NULL){
        if (temp->uid == uid)
            inode = temp;
        temp = temp->next_inode;
    }

    return inode;
}
