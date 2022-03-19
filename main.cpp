#include <cstdio>
#include "lib/storage.h"
#include "lib/inode.h"

int main() {
	// Welcome Screen
	printf("|------------------------------|\n");
	printf("|        OS Course Design      |\n");
	printf("|------------------------------|\n");
	printf("| Contributor:                 |\n");
	printf("|   Sizhe Zhong (201936234136) |\n");
	printf("|   Haohan Weng (201930251436) |\n");
	printf("|------------------------------|\n");
	printf("\n");

	// Initializers
	storageInitializer();
	inodeInitializer();

	// Main Program
	INode root = selectINode(1);
	printf("%d %d %lu\n", root.id, root.size, root.createAt);
	for (int i = 0; i < 10; ++i)
		printf("%d ", root.direct_addr[i]);
	printf("%d\n", root.indirect_addr);

	// Destructors
	storageDestructor();
	return 0;
}
