#ifndef _STORAGE_H
#define _STORAGE_H

const int MEMORY_SIZE = 1 << 24;
const int BLOCK_SIZE = 1 << 10;

bool getBlock(int addr, void* des, int byte_cnt = BLOCK_SIZE);
bool putBlock(int addr, const void* src, int byte_cnt = BLOCK_SIZE);
bool freeBlock(int addr);
int findFreeBlock();

void storageInitializer();
void storageDestructor();

#endif // _STORAGE_H
