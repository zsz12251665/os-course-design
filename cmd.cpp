#include "cmd.h"
#include "lib/storage.h"
#include <cstdio>

void cmd::sum() {
	int cnt = countFreeBlock();
	printf("Blocks  Used Available Use%% Mounted on\n");
	printf("%6d %5d %9d %3d%% %s\n", BLOCK_NUMBER, BLOCK_NUMBER - cnt, cnt, ((BLOCK_NUMBER - cnt) * 100 + BLOCK_NUMBER / 2) / BLOCK_NUMBER, "/");
}
