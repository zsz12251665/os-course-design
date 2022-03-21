#ifndef _STORAGE_H
#define _STORAGE_H

const int MEMORY_SIZE = 1 << 24;
const int BLOCK_SIZE = 1 << 10;
const int BLOCK_NUMBER = MEMORY_SIZE / BLOCK_SIZE;

struct StorageError {
	int addr;
	const char* message;
	const char* when;
};

int occupyBlock(); // Return the address of the newly occupied block
void freeBlock(int addr);

void getBlock(int addr, void* des, int byte_cnt = BLOCK_SIZE);
void putBlock(int addr, const void* src, int byte_cnt = BLOCK_SIZE);

int countFreeBlocks(); // Return the number of free blocks

void storageInitializer();
void storageDestructor();

#endif // _STORAGE_H
