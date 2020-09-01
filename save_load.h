#ifndef SAVE_LOAD
#define SAVE_LOAD

#include "parson.h"
#include "struct_SGF.h"

// ============ SECONDARY FUNCTIONS ============
Inode* append_inode_to_disk(Disk* disk, Inode* inode);
Inode* get_inode_by_uid(Disk* disk, uid_t uid);

DataBlock* append_datablock_to_list(DataBlock* list, DataBlock* block);
// =============================================

void save_disk(Disk* disk);
void load_disk(Disk* disk);

JSON_Value* save_inode(Inode* inode);
Inode* load_incomplete_inode(JSON_Object* object, size_t index);
void load_complete_inode(Disk* disk, JSON_Object* object, size_t index);

void save_datablocks(JSON_Object* json_object, Inode* inode);
DataBlock* load_datablocks(JSON_Object* json_object, Inode* inode);

char* save_dirblock(Inode* inode, DirectoryBlock* dir_block);
DirectoryBlock* load_dirblocks(Disk* disk, JSON_Object* object, Inode* inode);

JSON_Value* save_index(Index index);
Index load_index(Disk* disk, JSON_Object* object);

#endif