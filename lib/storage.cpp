#include "storage.h"
#include "inode.h"
#include <cstdio>
#include <cstring>

char memory[MEMORY_SIZE];

inline bool isValidAddress(int addr) {
	return 0 <= addr && addr < MEMORY_SIZE;
}

bool isFreeBlock(int block) {
	return (*(memory + block / 8) & (1 << (block % 8))) == 0;
}

void markBlockStatus(int block, bool isFree) {
	if (isFree)
		*(memory + block / 8) &= ~(1 << (block % 8));
	else
		*(memory + block / 8) |= 1 << (block % 8);
}

bool getBlock(int addr, void* des, int byte_cnt) {
	if (!isValidAddress(addr) || isFreeBlock(addr / BLOCK_SIZE))
		return 1;
	memcpy(des, memory + addr, byte_cnt);
	return 0;
}

bool putBlock(int addr, const void* src, int byte_cnt) {
	if (!isValidAddress(addr) || addr % BLOCK_SIZE + byte_cnt > BLOCK_SIZE)
		return 1;
	if (isFreeBlock(addr / BLOCK_SIZE))
		markBlockStatus(addr / BLOCK_SIZE, false); // Occupy the block
	memcpy(memory + addr, src, byte_cnt);
	return 0;
}

bool freeBlock(int addr) {
	if (!isValidAddress(addr))
		return 1;
	markBlockStatus(addr / BLOCK_SIZE, true); // Free the block
	return 0;
}

int findFreeBlock() {
	for (int i = INODE_MEMORY_END; i < MEMORY_SIZE; i += BLOCK_SIZE)
		if (isFreeBlock(i / BLOCK_SIZE))
			return i;
	return 0;
}

void storageInitializer() {
	auto f = fopen("os.memory.backup", "rb");
	if (f != NULL) {
		fread(memory, sizeof memory, 1, f);
		fclose(f);
	} else {
		memset(memory, 0, MEMORY_SIZE);
		markBlockStatus(0, false);
		markBlockStatus(1, false);
	}
}

void storageDestructor() {
	auto f = fopen("os.memory.backup", "wb");
	fwrite(memory, sizeof memory, 1, f);
	fclose(f);
}
