#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "struct_SGF.h"
#include "primitives.h"
#include "command_shell.h"
#include "save_load_SGF.h"
#include "constants.h"

void format_disk(Disk* disk){
	disk->inodes = NULL;
	disk->dir_blocks = NULL;
	disk->data_blocks = NULL;
	
	disk->nb_inode = 0;
	disk->nb_dir_blocks = 0;
	disk->nb_data_blocks = 0;
	
	mkdir("root", disk, NULL);
}

void addDataBlockHead(Data_block** head, Data_block block){

	Data_block* new_block = (Data_block*) malloc(sizeof(Data_block));

	strcpy(new_block->data, block.data);
	new_block->size = block.size;
	new_block->next_block = *head;
	new_block->prev_block = NULL;

	if (*head != NULL){
		(*head)->prev_block = new_block;
	}
	*head = new_block;
}

void addDirBlockHead(Directory_block** head, Directory_block block){

	Directory_block* new_block = (Directory_block*) malloc(sizeof(Directory_block));

	new_block->tab_index = block.tab_index;
	new_block->nb_index = block.nb_index;
	new_block->next_block = *head;
	new_block->prev_block = NULL;

	if (*head != NULL){
		(*head)->prev_block = new_block;
	}
	*head = new_block;
}

void addInodeHead(Inode** head, Inode inode){

	Inode* new_inode = (Inode*) malloc(sizeof(Inode));

	strcpy(new_inode->name, inode.name);
	strcpy(new_inode->permissions, inode.permissions);
	new_inode->type = inode.type;
	new_inode->date_creation = inode.date_creation;
	new_inode->date_modification = inode.date_modification;
	new_inode->data_blocks = inode.data_blocks;
	new_inode->dir_blocks = inode.dir_blocks;
	new_inode->nb_dir_blocks = inode.nb_dir_blocks;
	new_inode->nb_data_blocks = inode.nb_data_blocks;
	new_inode->next_inode = *head;
	new_inode->prev_inode = NULL;

	if (*head != NULL){
		(*head)->prev_inode = new_inode;
	}
	*head = new_inode;
}

int save_inode_dir_block_index(int index_index, int inode_index, int db_index, Index index){
	FILE* f = NULL;
	char* filename = (char*) malloc(sizeof(char) * 19);

	sprintf(filename, "index%dI%dDB%d.tmp", index_index, inode_index, db_index);
	f = fopen(filename, "w+");

	if (f == NULL){
		free(filename);
		return 1;
	}
	fprintf(f, "%s", index.name);
	#if (DEBUG_MODE==1)
	printf("saved index %d of inode %d directory block %d\n", index_index, inode_index, db_index);
	#endif
	fclose(f);
	free(filename);
	return 0;
}

int save_dir_block_index(int index_index, int db_index, Index index){
	FILE* f = NULL;
	char* filename = (char*) malloc(sizeof(char) * 15);

	sprintf(filename, "index%dDB%d.tmp", index_index, db_index);
	f = fopen(filename, "w+");

	if (f == NULL){
		free(filename);
		return 1;
	}

	fprintf(f, "%s", index.name);
	#if (DEBUG_MODE==1)
	printf("saved index %d of directory block %d\n", index_index, db_index);
	#endif
	fclose(f);
	free(filename);
	return 0;
}

int read_index(Inode* inode, FILE* f, Index* index){
	int return_value = fscanf(f, "%s", index->name);
	index->inode = inode;

	return return_value;
}

int save_inode_data_blocks(int inode_index, Data_block* blocks){
	if (blocks != NULL){
		FILE* f = NULL;
		char* filename = (char*) malloc(sizeof(char) * 19);

		Data_block* current = blocks;
		int sum = 0;
		while (current != NULL){
			sprintf(filename, "datablocks%dI%d.tmp", (++sum), inode_index);
			f = fopen(filename, "wb+");
			if (f == NULL){
				free(filename);
				return 1;
			}
			fwrite(&current->data, sizeof(char)*BUFFER_SIZE, 1, f);
			fwrite(&current->size, sizeof(int), 1, f);
			current = current->next_block;
		}
		#if (DEBUG_MODE==1)
		printf("saved %d data blocks of inode %d\n", sum, inode_index);
		#endif
		fclose(f);
		free(filename);
	}
	return 0;
}

int save_data_blocks(Data_block* blocks){
	FILE* f = NULL;
	char* filename = (char*) malloc(sizeof(char) * 16);

	Data_block* current = blocks->next_block;
	int sum = 0;
	while (current != NULL){
		sprintf(filename, "datablocks%d.tmp", (++sum));
		f = fopen(filename, "wb+");
		if (f == NULL){
			free(filename);
			return 1;
		}

		fwrite(&current->data, sizeof(char)*BUFFER_SIZE, 1, f);
		fwrite(&current->size, sizeof(int), 1, f);
		current = current->next_block;
	}
	#if (DEBUG_MODE==1)
	printf("saved %d data blocks\n", sum);
	#endif
	fclose(f);
	free(filename);
	return 0;
}

int read_data_block(FILE* f, Data_block* block){
	int return_value = fread(&block->data, sizeof(char)*BUFFER_SIZE, 1, f);
	return_value &= fread(&block->size, sizeof(int), 1, f);
	return return_value;
}
int load_data_blocks(Data_block** blocks, int nb_data_blocks){
	if (blocks != NULL){
		FILE* f = NULL;
		Data_block block;
		int sum = 0;
		char* filename = (char*) malloc(sizeof(char) * 16);

		for (int i = 0; i < nb_data_blocks; i++){
			sprintf(filename, "datablocks%d.tmp", (i+1));
			f = fopen(filename, "rb");
			if (f == NULL){
				free(filename);
				return 1;
			}

			if (read_data_block(f, &block) > 0) {
				addDataBlockHead(blocks, block);
			}
			sum = i+1;
			fclose(f);
		}
		#if (DEBUG_MODE==1)
		printf("loaded %d data blocks\n", sum);
		#endif

		free(filename);
	}
	return 0;
}

int load_inode_data_blocks(int inode_index, Data_block** blocks, int nb_data_blocks){
	if (nb_data_blocks > 0){
		FILE* f = NULL;
		Data_block block;
		int sum = 0;
		char* filename = (char*) malloc(sizeof(char) * 18);
		
		for (int i=0; i < nb_data_blocks; i++){
			sprintf(filename, "datablocks%dI%d.tmp", i, inode_index+1);
			f = fopen(filename, "rb");
			if (f == NULL){
				free(filename);
				return 0;
			}

			if (read_data_block(f, &block) > 0) {
				addDataBlockHead(blocks, block);
			}
			sum = i;
			fclose(f);
		}
		#if (DEBUG_MODE==1)
		printf("loaded %d data blocks of inode %d\n", sum, inode_index+1);
		#endif
		free(filename);
	} else {
		#if (DEBUG_MODE==1)
		printf("there is no data blocks for inode %d to be loaded\n", inode_index+1);
		#endif
	}
	return 1;
}

int load_inode_dir_blocks(Inode* inode, int inode_index, Directory_block** blocks){
	FILE* f = NULL;
	Directory_block block;
	int sum = 0;
	char* filename = (char*) malloc(sizeof(char) * 18);
	
	for (int i=0; i < inode->nb_dir_blocks; i++){
		sprintf(filename, "dirblocks%dI%d.tmp", (i+1), (inode_index+1));
		f = fopen(filename, "r");
		if (f == NULL){
			free(filename);
			return 0;
		}

		if (read_inode_dir_block(inode, f, &block, inode_index+1, i) > 0) {
			addDirBlockHead(blocks, block);
		}
		sum = i+1;
		fclose(f);
	}
	#if (DEBUG_MODE==1)
	printf("loaded %d dirblocks of inode %d\n", sum, inode_index);
	#endif
	free(filename);
	return 1;
}

int save_inode_dir_blocks(int inode_index, Directory_block* blocks){
	FILE* f = NULL;
	char* filename = (char*) malloc(sizeof(char) * 18);
	Directory_block* current = blocks;
	int sum = 0;
	while (current != NULL){
		sprintf(filename, "dirblocks%dI%d.tmp", (++sum), inode_index);
		f = fopen(filename, "w+");
		if (f == NULL){
			free(filename);
			return 1;
		}
		fprintf(f, "%d\n", current->nb_index);
		for (int i = 0; i < current->nb_index; i++){
			save_inode_dir_block_index(i, inode_index, sum, current->tab_index[i]);
		}
		current = current->next_block;
		fclose(f);
	}
	#if (DEBUG_MODE==1)
	printf("saved %d directory blocks of the inode %d\n", sum, inode_index);
	#endif
	free(filename);
	return 0;
}

int save_dir_blocks(Directory_block* blocks){
	FILE* f = NULL;
	char* filename = (char*) malloc(sizeof(char) * 15);

	Directory_block* current = blocks;
	int sum = 0;
	while (current != NULL){
		sprintf(filename, "dirblocks%d.tmp", (++sum));
		f = fopen(filename, "w+");
		if (f == NULL){
			free(filename);
			return 1;
		}
		fprintf(f, "%d\n", current->nb_index);
		for (int i = 0; i < current->nb_index; i++){
			save_dir_block_index(i, sum, current->tab_index[i]);
		}
		current = current->next_block;
	}
	#if (DEBUG_MODE==1)
	printf("saved %d directory blocks\n", sum);
	#endif
	fclose(f);
	free(filename);
	return 0;
}

int read_dir_block(FILE* f, Directory_block* block, int dir_index){
	FILE* index_file = NULL;
	char* filename = (char*) malloc(sizeof(char) * 18);
	
	int return_value = fscanf(f, "%d\n", (&block->nb_index));
	block->tab_index = (Index*) malloc(sizeof(Index) * block->nb_index);
	for (int i=0; i< block->nb_index; i++){
		sprintf(filename, "index%dDB%d.tmp", i, dir_index+1);
		index_file = fopen(filename, "r");
		if (index_file == NULL){
			free(filename);
			return 0;
		}
		return_value = read_index(NULL, index_file, &(block->tab_index[i]));
		fclose(index_file);
	}
	free(filename);
	return return_value;
}

int read_inode_dir_block(Inode* inode, FILE* f, Directory_block* block, int inode_index, int dir_index){
	FILE* index_file = NULL;
	char* filename = (char*) malloc(sizeof(char) * 18);
	
	int return_value = fscanf(f, "%d\n", (&block->nb_index));
	block->tab_index = (Index*) malloc(sizeof(Index) * block->nb_index);

	for (int i=0; i< block->nb_index; i++){
		sprintf(filename, "index%dI%dDB%d.tmp", i, inode_index, dir_index+1);
		index_file = fopen(filename, "r");
		if (index_file == NULL){
			free(filename);
			return 0;
		}
		return_value= read_index(inode, index_file, &(block->tab_index[i]));
		fclose(index_file);
	}
	free(filename);
	return return_value;
}

int load_dir_blocks(Directory_block** blocks, int nb_dir_blocks){

	FILE* f = NULL;
	Directory_block block;
	int sum = 0;
	char* filename = (char*) malloc(sizeof(char) * 15);

	for (int i = 0; i < nb_dir_blocks; i++){
		sprintf(filename, "dirblocks%d.tmp", (i+1));
		f = fopen(filename, "r");
		if (f == NULL){
			free(filename);
			return 1;
		}

		if (read_dir_block(f, &block, i) > 0) {
			addDirBlockHead(blocks, block);
		}
		sum = i+1;
		fclose(f);
	}
	#if (DEBUG_MODE==1)
	printf("loaded %d dirblocks\n", sum);
	#endif
	free(filename);
	return 0;
}

int save_inodes(Inode* inodes){
	FILE* f = NULL;
	Inode* current = NULL;
	int sum = 0;
	char* filename = (char*) malloc(sizeof(char) * 12);

	current = inodes;
	while (current != NULL){
		sprintf(filename, "inodes%d.tmp", (++sum));
		f = fopen(filename, "w+");
		if (f == NULL){
			free(filename);
			return 1;
		}
		fprintf(f, "%s\n", current->name);
		fprintf(f, "%s\n", current->permissions);
		fprintf(f, "%d\n", current->type);
		fprintf(f, "%ld\n", current->date_creation);
		fprintf(f, "%ld\n", current->date_modification);
		fprintf(f, "%d\n", current->nb_dir_blocks);
		fprintf(f, "%d\n", current->nb_data_blocks);
		
		save_inode_dir_blocks(sum, current->dir_blocks);
		// TODO: save_inode_data_blocks needs to be changed!
		//save_inode_data_blocks(sum, current->data_blocks);
		current = current->next_inode;
	}
	#if (DEBUG_MODE==1)
	printf("saved %d inodes\n", sum);
	#endif
	free(filename);
	fclose(f);
	return 0;
}

int read_inode(FILE* f, int inode_index, Inode* inode){
	int return_value = fscanf(f, "%s\n", inode->name);
	return_value &= fscanf(f, "%s\n", inode->permissions);
	return_value &= fscanf(f, "%d\n", &inode->type);
	return_value &= fscanf(f, "%ld\n", &inode->date_creation);
	return_value &= fscanf(f, "%ld\n", &inode->date_modification);
	return_value &= fscanf(f, "%d\n", &inode->nb_dir_blocks);
	return_value &= fscanf(f, "%d\n", &inode->nb_data_blocks);

	inode->dir_blocks = NULL;
	inode->data_blocks = NULL;
	return_value &= load_inode_dir_blocks(inode, inode_index, &inode->dir_blocks);
	// TODO : load_inode_data_blocks needs to be changed!
	//return_value &= load_inode_data_blocks(inode_index, &inode->data_blocks, inode->nb_data_blocks);
	return return_value;
}

int load_inodes(Inode** inodes, int nb_inodes){
	FILE* f = NULL;
	Inode* inode = (Inode*) malloc(sizeof(Inode));
	inode->next_inode = NULL;
	inode->prev_inode = NULL;
	int sum = 0;
	char* filename = (char*) malloc(sizeof(char) * 12);

	for (int i = 0; i < nb_inodes; i++){
		sprintf(filename, "inodes%d.tmp", (i+1));
		f = fopen(filename, "r");
		if (f == NULL){
			free(filename);
			return 1;
		}
		if (read_inode(f, i, inode) > 0) {
			addInodeHead(inodes, *inode);
		}
		sum = i + 1;
		fclose(f);
	}
	#if (DEBUG_MODE==1)
	printf("loaded %d inodes\n", sum);
	#endif
	free(filename);
	return 0;
}

int save_disk(Disk disk){
	FILE *file = fopen(DISK_FILE_NAME, "wb");

	if (file == NULL || fwrite(&disk, sizeof(Disk), 1, file) <= 0)
		return 0;
		
	fclose(file);

	// save inodes
	if (disk.inodes != NULL && save_inodes(disk.inodes) == 1){
		printf("Error saving the inodes of the disk!\n");
		return 0;
	}

	// save dir blocks
	if (disk.dir_blocks != NULL && save_dir_blocks(disk.dir_blocks) == 1){
		printf("Error saving the directory blocks of the disk!\n");
		return 0;
	}
	
	// TODO: save_data_blocks going to be changed!
	// save data blocks
	//if (disk.data_blocks != NULL && save_data_blocks(disk.data_blocks) == 1){
	//	printf("Error saving the data blocks of the disk!\n");
	//	return 0;
	//}

	return 1;
}

int file_exists(char* filename){
	FILE* f = NULL;
	f = fopen(filename, "rb");
	if (f == NULL)
		return 0;
	fclose(f);
	return 1;
}

int disk_exists(){
	return file_exists(DISK_FILE_NAME);
}

#if (DEBUG_MODE==1)
void d_print_inodes(Inode* inodes){
	if (inodes != NULL){
		Inode* temp = inodes;
		do {
			printf("name %s\n", temp->name);
			printf("permissions %s\n", temp->permissions);
			printf("type %d\n", temp->type);
			printf("creation date %ld\n", temp->date_creation);
			printf("modification date %ld\n", temp->date_modification);
			printf("nb dir blocks %d\n", temp->nb_dir_blocks);
			printf("nb data blocks %d\n", temp->nb_data_blocks);
			temp = temp->next_inode;
		}while (temp != NULL);
	} else {
		printf("inodes are null\n");
	}
}

void d_print_indexes(Index* indexes, int nb_indexes){
	if (indexes != NULL || nb_indexes == 0){
		for (int i = 0; i < nb_indexes; i++){
			printf("index name %s\n", indexes[i].name);
			printf("inode name %s\n", indexes[i].inode->name);
		}
	} else {
		printf("indexes are null\n");
	}
}

void d_print_dirblocks(Directory_block* dirblocks){
	if (dirblocks != NULL){
		Directory_block* temp = dirblocks;
		do {
			printf("nbindex %d\n", temp->nb_index);
			printf("*-*-*-*-*-*-*-*\n");
			d_print_indexes(temp->tab_index, temp->nb_index);
			printf("*-*-*-*-*-*-*-*\n");
			temp = temp->next_block;
		}while (temp != NULL);
	} else {
		printf("dirblocks are null\n");
	}
}
#endif

int load_disk(Disk* disk){
	FILE* f = NULL;
	f = fopen(DISK_FILE_NAME, "rb");
	
	if (f == NULL) {
		printf("DISK FILE not found!\n");
		return 0;
	}
	if (fread(disk, sizeof(Disk), 1, f) <= 0)
		return 0;

	fclose(f);

	// load the inodes
	disk->inodes = NULL;
	if (load_inodes(&disk->inodes, disk->nb_inode) == 1){
		printf("The loading of the disk inodes has failed!\n");
		return 0;
	}
	// load the dir blocks
	disk->dir_blocks = NULL;
	if (load_dir_blocks(&disk->dir_blocks, disk->nb_dir_blocks) == 1){
		printf("The loading of the disk dir_blocks has failed!\n");
		return 0;
	}


	// TODO: load_data_blocks goign to be changed!
	// load the data blocks
	//if (file_exists("datablocks1.tmp")){
	//	disk->data_blocks = NULL;
	//	if (load_data_blocks(&disk->data_blocks, disk->nb_data_blocks) ==1){
	//		printf("The loading of the disk data_blocks has failed!\n");
	//		return 0;
	//	}
	//}

	// Cleaning related-disk files
	system("rm *.tmp *.dat");

	return 1;
}

Directory_block* allocation_block_directory(){
	Directory_block* tab_block = NULL;
	
	tab_block=(Directory_block*) malloc(sizeof(Directory_block));
	tab_block->next_block = NULL;
	
	return tab_block;
}

Data_block* allocation_block_data(){
	Data_block* block = NULL;
	
	block=(Data_block*) malloc(sizeof(Data_block));
	block->next_block = NULL;
		
	return block;
}

Data_block** allocation_tab_block_data(int size){
	Data_block** tab_block = NULL;
	
	tab_block=(Data_block**) malloc(size*sizeof(Data_block*));
	
	tab_block[0] = allocation_block_data();
	
	return tab_block;
}

void init_block_directory(Directory_block* block,Inode* inode_directory,Inode* inode_parent_directory,Disk* disk){
	char name_parent_dir[] = "..";
	char name_dir[] = ".";
	
	block->tab_index = NULL;
	block->tab_index = allocation_index(2);
	block->nb_index = 2;
	
	strcpy(block->tab_index[0].name,name_dir);
	block->tab_index[0].inode=inode_directory;
	
	strcpy(block->tab_index[1].name,name_parent_dir);
	block->tab_index[1].inode=inode_parent_directory;
	
	block->next_block = NULL;
	add_dir_block(block,disk);

}

void init_block_data(Data_block* block,Disk* disk){

	block->size = 0;
	
	block->next_block = NULL;
	add_data_block(block,disk);

}

Index* allocation_index(int size){
	Index* index=NULL;
	
	index=(Index*)malloc(size*sizeof(Index));
	
	return index;
}

void init_permissions(char permissions[10]){
	permissions[0]='r';
	permissions[1]='w';
	permissions[2]='x';
	permissions[3]='r';
	permissions[4]='-';
	permissions[5]='-';
	permissions[6]='r';
	permissions[7]='-';
	permissions[8]='-';
	permissions[9]='\0';
}

void delete_permissions(char permissions[10]){
	permissions[0]='-';
	permissions[1]='-';
	permissions[2]='-';
	permissions[3]='-';
	permissions[4]='-';
	permissions[5]='-';
	permissions[6]='-';
	permissions[7]='-';
	permissions[8]='-';
}

void free_inode(Disk* disk,Inode* inode){
	if(inode->prev_inode != NULL) {
		inode->prev_inode->next_inode=inode->next_inode;
	}
	if(inode->next_inode != NULL) {
		inode->next_inode->prev_inode = inode->prev_inode;
	}
	if(inode->prev_inode == NULL) {
		disk->inodes = inode->next_inode;
	}


	if(inode->type == DIRECTORY){ 
		free_block_directory(disk,inode->dir_blocks);
		inode->dir_blocks = NULL;
	} else {
		for(int i=0;i<inode->nb_data_blocks;i++) {
			free_block_data(disk,inode->data_blocks[i]);
		}
		free(inode->data_blocks);
		inode->data_blocks = NULL;
	}
	
	inode->prev_inode = NULL;
	inode->next_inode = NULL;
	
	free(inode);
	
	disk->nb_inode--;
	
}

void free_block_directory(Disk* disk, Directory_block* block){

	if (block->prev_block != NULL) {
		block->prev_block->next_block=block->next_block;
	}
	if(block->next_block != NULL) {
		block->next_block->prev_block = block->prev_block;
	}
	if(block->prev_block == NULL) {
		disk->dir_blocks = block->next_block;
	}
	
	free(block->tab_index);
	free(block);
	
	disk->nb_dir_blocks--;

}

void free_block_data(Disk* disk, Data_block* block){

	if(block->prev_block != NULL) {
		block->prev_block->next_block=block->next_block;
	}
	if(block->next_block != NULL) {
		block->next_block->prev_block = block->prev_block;
	}
	if(block->prev_block == NULL) {
		disk->data_blocks = block->next_block;
	}
	
	free(block);
	
	disk->nb_data_blocks--;	
}

void free_disk(Disk* disk){
	if(disk->inodes != NULL){
		Inode* current_inode = disk->inodes;
		
		while(current_inode->next_inode != NULL){
			current_inode = current_inode->next_inode;
			free_inode(disk,current_inode->prev_inode);
		}
		free_inode(disk,current_inode);
	}
}

Inode* get_last_inode(Disk disk){
	Inode* current_inode = disk.inodes;
	
	while(current_inode != NULL && current_inode->next_inode != NULL){
		current_inode = current_inode->next_inode;
	}
	
	return current_inode;
}

void add_inode(Inode* inode, Disk* disk){
	if(disk->inodes != NULL){
		Inode* last_inode = get_last_inode(*disk);
		
		last_inode->next_inode = inode;
		inode->prev_inode = last_inode;
		
		disk->nb_inode++;
	} 
	else{
		disk->inodes = inode;
		inode->prev_inode = NULL;
		disk->nb_inode = 1;
	}
	
}

Directory_block* get_last_dir_block(Disk disk){
	Directory_block* current_dir_block = disk.dir_blocks;
	
	while(current_dir_block != NULL && current_dir_block->next_block != NULL){
		current_dir_block = current_dir_block->next_block;
	}
	
	return current_dir_block;
}

void add_dir_block(Directory_block* dir_block, Disk* disk){
	if(disk->dir_blocks != NULL){
		Directory_block* last_dir_block = get_last_dir_block(*disk);
		
		last_dir_block->next_block = dir_block;
		dir_block->prev_block = last_dir_block;

		disk->nb_dir_blocks++;
	}
	else{
		disk->dir_blocks = dir_block;
		dir_block->prev_block = NULL;
		disk->nb_dir_blocks = 1;
	}
	
}
	
Data_block* get_last_data_block(Disk disk){
	Data_block* current_data_block = disk.data_blocks;
	
	while(current_data_block != NULL && current_data_block->next_block != NULL){
		current_data_block = current_data_block->next_block;
	}
	
	return current_data_block;
}

void add_data_block(Data_block* data_block, Disk* disk){
	if(disk->data_blocks != NULL){
		Data_block* last_data_block = get_last_data_block(*disk);
		
		last_data_block->next_block = data_block;
		data_block->prev_block = last_data_block;
		
		disk->nb_data_blocks++;
	}
	else{
		disk->data_blocks = data_block;
		data_block->prev_block = NULL;
		disk->nb_data_blocks = 1;
	}
	
}

void update_tab_index(Inode* current_inode, Inode* inode_to_add){
	Index* new_index = NULL;
	int i;
	
	new_index = allocation_index(current_inode->dir_blocks->nb_index+1);
	
	for(i=0; i<current_inode->dir_blocks->nb_index;i++){
		new_index[i] = current_inode->dir_blocks->tab_index[i]; //copy the old index in the new one
	}
	
	//add the directory at the end of the index
	strcpy(new_index[current_inode->dir_blocks->nb_index].name,inode_to_add->name);
	new_index[current_inode->dir_blocks->nb_index].inode = inode_to_add;
	
	free(current_inode->dir_blocks->tab_index);
	current_inode->dir_blocks->tab_index = new_index;
	
	current_inode->dir_blocks->nb_index++;
	
}

void remove_tab_index(Inode* inode_to_remove,Disk* disk){
	Index* new_index = NULL;
	int i;
	int j = 0;
	Inode* parent_inode = search_parent_inode(inode_to_remove,disk);
	
	new_index = allocation_index(parent_inode->dir_blocks->nb_index-1);

	
	for(i=0;i<parent_inode->dir_blocks->nb_index;i++){
		if(parent_inode->dir_blocks->tab_index[i].inode != inode_to_remove) {
			new_index[j] = parent_inode->dir_blocks->tab_index[i]; //copy the old index in the new one without the inode to remove
			j++;
		}
	}
	
	free(parent_inode->dir_blocks->tab_index);
	parent_inode->dir_blocks->tab_index = new_index;
	
	parent_inode->dir_blocks->nb_index--;

}

Inode* search_file_in_directory(char* file_name,Directory_block* directory) {
	int i;
	
	if(directory != NULL) {
		for(i=0;i<directory->nb_index;i++) {
			if(strcmp(directory->tab_index[i].name,file_name) == 0) {
				return directory->tab_index[i].inode;
			}
		}
	}
	return NULL;
}

Inode* search_parent_inode(Inode* inode,Disk* disk) {
	Inode* parent_inode = disk->inodes;
	Directory_block* directory;
	int i;
	
	while(parent_inode != NULL) {
		if(parent_inode->dir_blocks != NULL) {
			directory = parent_inode->dir_blocks;
			if(directory->nb_index > 2) {
				for(i=0;i<directory->nb_index;i++) {
					if(directory->tab_index[i].inode == inode) {
						return parent_inode;
					}
				}
			}
		}
		parent_inode = parent_inode->next_inode;
	}
	return NULL;
}
					


