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
        char* inode_label = malloc(sizeof(char) * 16);
        sprintf(inode_label, "inode_%d", inode->uid);

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
    print_debug("datablock data buffer: %s", buffer);
    json_object_set_value(json_object, "datablock", json_parse_string(buffer));
    
    free(buffer);
    free(block);
}

char* save_dirblock(Inode* inode, DirectoryBlock* dir_block){
    char* buffer = (char*) malloc(sizeof(char*) * 65556);
    
    strcpy(buffer, "[");
    for (int i = 0; i < dir_block->nb_index; i++){
        JSON_Value* index_json_value = save_index(dir_block->tab_index[i]);

        char* serialized_string = json_serialize_to_string(index_json_value);
        strcat(buffer, serialized_string);
            
        json_free_serialized_string(serialized_string);
        json_value_free(index_json_value);

        if (i + 1 < dir_block->nb_index)
            strcat(buffer, ",");
           
    }
    strcat(buffer, "]");
    

    return buffer;
}

JSON_Value* save_index(Index index){
    JSON_Value* root_value = json_value_init_object();
    JSON_Object* root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "name", index.name);
    json_object_set_number(root_object, "inode_uid", index.inode->uid);

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

        //inode->prev_inode = temp;
        temp->next_inode = inode;
    }
    return disk->inodes;
}

Inode* load_incomplete_inode(JSON_Object* object, size_t index) {
    JSON_Value* inode_json_value = json_object_get_value_at(object, index);
    JSON_Object* inode_json_object = json_value_get_object(inode_json_value);

    Inode* inode = (Inode*) malloc(sizeof(Inode));
    inode->uid = (int) json_object_get_number(inode_json_object, "uid");
    strcpy(inode->name, json_object_get_string(inode_json_object, "name"));
    strcpy(inode->permissions, json_object_get_string(inode_json_object, "permissions"));
    inode->type = (int)json_object_get_number(inode_json_object, "type");
    inode->creation_date = (time_t)json_object_get_number(inode_json_object, "creation_date");
    inode->modification_date = (time_t)json_object_get_number(inode_json_object, "modification_date");
    //inode->prev_inode = NULL;
    inode->next_inode = NULL;

    if (inode->type <= 2) {
        inode->datablocks = load_datablocks(inode_json_object, inode);
    }
    inode->dirblock = NULL;

    return inode;
}

void load_complete_inode(Disk* disk, JSON_Object* object, size_t index) {
    JSON_Value* inode_json_value = json_object_get_value_at(object, index);
    JSON_Object* inode_json_object = json_value_get_object(inode_json_value);

    // get the inode from the disk by its uid
    int inode_uid = (int) json_object_get_number(inode_json_object, "uid");
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
    } else {
        #if DEBUG
        printf(HIGHTLIGHT"there is %ld datablocks to be loaded from inode %d"RESET"\n", block_count, inode->uid);
        #endif
    }

    DataBlock* blocks = NULL;
    
    for (size_t i = 0; i < block_count; i++) {
        DataBlock* new_block = malloc(sizeof(DataBlock));
        
        strcpy(new_block->data, json_array_get_string(data_json_array, i));
        new_block->prev_block = NULL;
        new_block->next_block = NULL;
        
        blocks = append_datablock_to_list(blocks, new_block);
    }

    // #if DEBUG == 1
    // DataBlock* current = blocks;
    // int size = 0;
    // while (current != NULL) {
    //     size++;
    //     printf(HIGHTLIGHT"datablock[data]: %s"RESET"\n", current->data);
    //     current = current->next_block;
    // }
    // printf(HIGHTLIGHT"there is %d blocks!"RESET"\n", size);
    // #endif

    return blocks;
}

DataBlock* append_datablock_to_list(DataBlock* list, DataBlock* block){
    if (list == NULL){
        list = block;

    } else if (block != NULL) {
        DataBlock* temp = list;

        while (temp->next_block != NULL){
            temp = temp->next_block;
        }

        block->prev_block = temp;
        temp->next_block = block;

        // TODO free?
    }
    return list;
}

DirectoryBlock* load_dirblocks(Disk* disk, JSON_Object* object, Inode* inode) {
    DirectoryBlock* block = malloc(sizeof(DirectoryBlock));
    JSON_Array* indexes_json = json_object_get_array(object, "dirblock");
    
    size_t count = json_array_get_count(indexes_json);
    block->nb_index = count;
    block->tab_index = (Index*) malloc(sizeof(Index) * count);

    for (size_t i = 0; i < count; i++){
        JSON_Object* index_json_object = json_array_get_object(indexes_json, i);
        block->tab_index[i] = load_index(disk, index_json_object);
    }

    return block;
}

Index load_index(Disk* disk, JSON_Object* object) {
    Index index;

    strcpy(index.name, json_object_get_string(object, "name"));
    int inode_id = (int) json_object_get_number(object, "inode_uid");

    index.inode = get_inode_by_uid(disk, inode_id);

    return index;
}

Inode* get_inode_by_uid(Disk* disk, int uid){
    Inode* inode = NULL;
    Inode* temp = disk->inodes;

    while (inode == NULL && temp != NULL){
        if (temp->uid == uid)
            inode = temp;
        temp = temp->next_inode;
    }

    return inode;
}
