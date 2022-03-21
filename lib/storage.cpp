#include "storage.h"
#include "inode.h"
#include <cstdio>
#include <cstring>

char memory[MEMORY_SIZE];

inline bool isValidAddress(int addr) {
	return 0 <= addr && addr < MEMORY_SIZE;
}

inline bool isFreeBlock(int block) {
	return (*(memory + block / 8) & (1 << (block % 8))) == 0;
}

int occupyBlock() {
	for (int addr = INODE_MEMORY_END; addr < MEMORY_SIZE; addr += BLOCK_SIZE)
		if (isFreeBlock(addr / BLOCK_SIZE)) {
			memset(memory + addr, 0, BLOCK_SIZE);
			int block = addr / BLOCK_SIZE;
			*(memory + block / 8) |= 1 << (block % 8);
			return addr;
		}
	throw StorageError({MEMORY_SIZE, "Cannot find a free block!", "occupyBlock"});
}

void freeBlock(int addr) {
	if (!isValidAddress(addr))
		throw StorageError({addr, "Address is invalid!", "freeBlock"});
	int block = addr / BLOCK_SIZE;
	*(memory + block / 8) &= ~(1 << (block % 8));
}

void getBlock(int addr, void* des, int byte_cnt) {
	if (!isValidAddress(addr))
		throw StorageError({addr, "Address is invalid!", "getBlock"});
	if (isFreeBlock(addr / BLOCK_SIZE))
		throw StorageError({addr, "Target block is not occupied!", "getBlock"});
	if (addr % BLOCK_SIZE + byte_cnt > BLOCK_SIZE)
		throw StorageError({addr, "Content overflows the block!", "getBlock"});
	memcpy(des, memory + addr, byte_cnt);
}

void putBlock(int addr, const void* src, int byte_cnt) {
	if (!isValidAddress(addr))
		throw StorageError({addr, "Address is invalid!", "putBlock"});
	if (isFreeBlock(addr / BLOCK_SIZE))
		throw StorageError({addr, "Target block is not occupied!", "putBlock"});
	if (addr % BLOCK_SIZE + byte_cnt > BLOCK_SIZE)
		throw StorageError({addr, "Content overflows the block!", "putBlock"});
	memcpy(memory + addr, src, byte_cnt);
}

int countFreeBlocks() {
	int cnt = 0;
	for (int i = 0 ; i < BLOCK_NUMBER; ++i)
		if (isFreeBlock(i))
			++cnt;
	return cnt;
}

void storageInitializer() {
	auto f = fopen("os.memory.backup", "rb");
	if (f != NULL) {
		fread(memory, MEMORY_SIZE, 1, f);
		fclose(f);
	} else {
		memset(memory, 0, MEMORY_SIZE);
		// Reserve blocks for block bitset & INode area
		for (int block = 0; block < INODE_MEMORY_END / BLOCK_SIZE; ++block)
			*(memory + block / 8) |= 1 << (block % 8);
	}
}

void storageDestructor() {
	auto f = fopen("os.memory.backup", "wb");
	fwrite(memory, MEMORY_SIZE, 1, f);
	fclose(f);
}
