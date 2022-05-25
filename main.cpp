#include <cstdio>
#include <cstring>
#include "lib/storage.h"
#include "lib/inode.h"
#include "cmd.h"

const int PATH_MAX_LENGTH = 1 << 8;

void normalizePath(char* path) {
	int len = strlen(path);
	while (len > 1 && path[len - 1] == '/') {
		path[len - 1] = '\0';
		--len;
	}
	while (len > 1 && path[len - 1] == '.') {
		if (path[len - 2] == '/') {
			path[len == 2 ? 1 : len - 2] = '\0';
			len -= 2;
		} else {
			break;
		}
	}
}

int main() {
	// Welcome Screen
	printf("|--------------------------------|\n");
	printf("|        OS Course Design        |\n");
	printf("|--------------------------------|\n");
	printf("| Contributor:                   |\n");
	printf("|   Sizhe Zhong (201936234136)   |\n");
	printf("|   Haohan Weng (201930251436)   |\n");
	printf("|   Hongqiao Lian (201930342141) |\n");
	printf("|--------------------------------|\n");
	printf("\n");

	// Initializers
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
		if (strcmp(command, "createFile") == 0
		|| strcmp(command, "new") == 0) {
			char filename[PATH_MAX_LENGTH];
			int size;
			scanf("%s%d", filename, &size);
			normalizePath(filename);
			cmd::createFile(filename, size);
		} else
		if (strcmp(command, "deleteFile") == 0
		|| strcmp(command, "rm") == 0) {
			char filename[PATH_MAX_LENGTH];
			scanf("%s", filename);
			normalizePath(filename);
			cmd::deleteFile(filename);
		} else
		if (strcmp(command, "createDir") == 0
		|| strcmp(command, "mkdir") == 0) {
			char dirname[PATH_MAX_LENGTH];
			scanf("%s", dirname);
			normalizePath(dirname);
			cmd::createDir(dirname);
		} else
		if (strcmp(command, "deleteDir") == 0
		|| strcmp(command, "rmdir") == 0) {
			char dirname[PATH_MAX_LENGTH];
			scanf("%s", dirname);
			normalizePath(dirname);
			cmd::deleteDir(dirname);
		} else
		if (strcmp(command, "changeDir") == 0
		|| strcmp(command, "cd") == 0) {
			char dirname[PATH_MAX_LENGTH];
			scanf("%s", dirname);
			normalizePath(dirname);
			cmd::changeDir(dirname);
		} else
		if (strcmp(command, "dir") == 0
		|| strcmp(command, "ls") == 0) {
			cmd::dir();
		} else
		if (strcmp(command, "cp") == 0
		|| strcmp(command, "copy") == 0) {
			char src[PATH_MAX_LENGTH], des[PATH_MAX_LENGTH];
			scanf("%s%s", src, des);
			normalizePath(src);
			normalizePath(des);
			cmd::cp(src, des);
		} else
		if (strcmp(command, "sum") == 0
		|| strcmp(command, "df") == 0) {
			cmd::sum();
		} else
		if (strcmp(command, "cat") == 0
		|| strcmp(command, "print") == 0) {
			char filename[PATH_MAX_LENGTH];
			scanf("%s", filename);
			normalizePath(filename);
			cmd::cat(filename);
		} else
		if (strcmp(command, "exit") == 0
		|| strcmp(command, "quit") == 0) {
			break;
		} else {
			printf("Error: Unknown command \"%s\"!\n", command);
		}
		while (getchar() != '\n'); // Clear the rest of the line in stdin
	}

	// Destructors
	storageDestructor();
	return 0;
}
