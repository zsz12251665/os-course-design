#include "cmd.h"
#include "lib/storage.h"
#include <cstdio>

inline int lowbit(int x) {
	return x & (-x);
}

void cmd::sum() {
	int counter = 0;
	int block[BLOCK_SIZE / 4];
	getBlock(0, block);
	for (int i = 0; i < BLOCK_SIZE / 4; ++i)
		for (; block[i] != 0; block[i] ^= lowbit(block[i]))
			++counter;
	printf("%9s %9s %9s %3s%% %s\n", "1K-blocks", "Used", "Available", "Use", "Mounted on");
	printf("%9d %9d %9d %3d%% %s\n", MEMORY_SIZE, counter, MEMORY_SIZE - counter, (counter * 100 + MEMORY_SIZE / 2) / MEMORY_SIZE, "/");
}
