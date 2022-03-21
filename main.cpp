#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include "lib/storage.h"
#include "lib/inode.h"
#include "cmd.h"

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
	srand(time(NULL));
	storageInitializer();
	inodeInitializer();
	commandInitializer();

	// Main Program
	for (;;) {
		printf("$ ");
		char command[PATH_MAX_LENGTH];
		if (scanf("%s", command) == EOF) {
			putchar('\n');
			break;
		}
		if (strcmp(command, "createFile") == 0) {
			char filename[PATH_MAX_LENGTH];
			int size;
			scanf("%s%d", filename, &size);
			cmd::createFile(filename, size);
		} else
		if (strcmp(command, "deleteFile") == 0) {
			char filename[PATH_MAX_LENGTH];
			scanf("%s", filename);
			cmd::deleteFile(filename);
		} else
		if (strcmp(command, "createDir") == 0) {
			char dirname[PATH_MAX_LENGTH];
			scanf("%s", dirname);
			cmd::createDir(dirname);
		} else
		if (strcmp(command, "deleteDir") == 0) {
			char dirname[PATH_MAX_LENGTH];
			scanf("%s", dirname);
			cmd::deleteDir(dirname);
		} else
		if (strcmp(command, "changeDir") == 0) {
			char dirname[PATH_MAX_LENGTH];
			scanf("%s", dirname);
			cmd::changeDir(dirname);
		} else
		if (strcmp(command, "dir") == 0) {
			cmd::dir();
		} else
		if (strcmp(command, "cp") == 0) {
			char src[PATH_MAX_LENGTH], des[PATH_MAX_LENGTH];
			scanf("%s%s", src, des);
			cmd::cp(src, des);
		} else
		if (strcmp(command, "sum") == 0) {
			cmd::sum();
		} else
		if (strcmp(command, "cat") == 0) {
			char filename[PATH_MAX_LENGTH];
			scanf("%s", filename);
			cmd::cat(filename);
		} else {
			printf("Error: Unknown command \"%s\"!\n", command);
		}
		while (getchar() != '\n'); // Clear the rest of the line in stdin
	}

	// Destructors
	storageDestructor();
	return 0;
}
