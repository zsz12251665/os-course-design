#ifndef _STORAGE_H
#define _STORAGE_H

const int MEMORY_SIZE = 1 << 24;
const int BLOCK_SIZE = 1 << 10;

bool getBlock(int addr, void* block, int byte_cnt = BLOCK_SIZE);
bool putBlock(int addr, void* block, int byte_cnt = BLOCK_SIZE);
bool freeBlock(int addr);

void memoryInitializer();
void memoryDestructor();

#endif // _STORAGE_H
