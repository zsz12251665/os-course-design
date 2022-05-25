#include "cmd.h"
#include "lib/storage.h"
#include "lib/inode.h"
#include "lib/directory.h"
#include "lib/file.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

int cwd_inode_num;

const char* getFilename(const char* path) {
	int len = strlen(path);
	for (int i = len - 1; i >= 0; --i)
		if (path[i] == '/')
			return path + i + 1;
	return path;
}

void cmd::createFile(const char* filename, int size) {
	if (size > 266) {
		printf("Error: The file is too big!\n");
		return;
	}
	if (countFreeBlocks() < (size > 10 ? size + 1 : size)) {
		printf("Error: Not enough space!\n");
		return;
	}
	if (strlen(getFilename(filename)) > FILENAME_MAX_LENGTH) {
		printf("Error: The filename is too long!\n");
		return;
	}
	INode dir_inode = getDirINode(selectINode(cwd_inode_num), filename);
	if (dir_inode.num == 0) {
		printf("Error: The path does not exist!\n");
		return;
	}
	if (getFileINode(selectINode(cwd_inode_num), filename).num != 0) {
		printf("Error: \"%s\" already exists!\n", filename);
		return;
	}
	INode file_inode = createFile(dir_inode, getFilename(filename));
	fillFile(file_inode, size * 1024);
}

void cmd::deleteFile(const char* filename) {
	INode file_inode = getFileINode(selectINode(cwd_inode_num), filename);
	if (file_inode.num == 0) {
		printf("Error: \"%s\" does not exist!\n", filename);
		return;
	}
	if (file_inode.type != FILE_INODE_TYPE) {
		printf("Error: \"%s\" is not a file!\n", filename);
		return;
	}
	INode dir_inode = getDirINode(selectINode(cwd_inode_num), filename);
	deleteFile(dir_inode, getFilename(filename));
}

void cmd::createDir(const char* dirname) {
	if (countFreeBlocks() == 0) {
		printf("Error: Not enough space!\n");
		return;
	}
	if (strlen(getFilename(dirname)) > FILENAME_MAX_LENGTH) {
		printf("Error: The direname is too long!\n");
		return;
	}
	INode dir_inode = getDirINode(selectINode(cwd_inode_num), dirname);
	if (dir_inode.num == 0) {
		printf("Error: The path does not exist!\n");
		return;
	}
	if (getFileINode(selectINode(cwd_inode_num), dirname).num != 0) {
		printf("Error: \"%s\" already exists!\n", dirname);
		return;
	}
	INode file_inode = createFile(dir_inode, getFilename(dirname));
	initDir(file_inode, dir_inode.num);
}

void cmd::deleteDir(const char* dirname) {
	INode file_inode = getFileINode(selectINode(cwd_inode_num), dirname);
	if (file_inode.num == 0) {
		printf("Error: \"%s\" does not exist!\n", dirname);
		return;
	}
	if (file_inode.type != DIR_INODE_TYPE) {
		printf("Error: \"%s\" is not a directory!\n", dirname);
		return;
	}
	if (file_inode.num == cwd_inode_num) {
		printf("Error: CWD could not be removed!\n");
		return;
	}
	if (file_inode.num == ROOT_INODE_NUM) {
		printf("Error: / could not be removed!\n");
		return;
	}
	if (file_inode.size > 2 * DIR_ENTRY_SIZE) {
		printf("Error: \"%s\" is not empty!\n", dirname);
		return;
	}
	INode dir_inode = getDirINode(selectINode(cwd_inode_num), dirname);
	deleteFile(dir_inode, getFilename(dirname));
}

void cmd::changeDir(const char* dirname) {
	INode dir_inode = getFileINode(selectINode(cwd_inode_num), dirname);
	if (dir_inode.num == 0) {
		printf("Error: \"%s\" does not exist!\n", dirname);
		return;
	}
	if (dir_inode.type != DIR_INODE_TYPE) {
		printf("Error: \"%s\" is not a directory!\n", dirname);
		return;
	}
	cwd_inode_num = dir_inode.num;
}

void cmd::dir() {
	AddrBlock cache;
	INode cwd_inode = selectINode(cwd_inode_num);
	printf("   Type              Create Time     Size Filename\n");
	for (int logic_addr = 0; logic_addr < cwd_inode.size; logic_addr += DIR_ENTRY_SIZE) {
		DirEntry entry;
		getBlock(cwd_inode.convertAddress(logic_addr, cache), &entry, DIR_ENTRY_SIZE);
		INode inode = selectINode(entry.inode_num);
		auto type_str = inode.type == DIR_INODE_TYPE ? "DIR"
			: inode.type == FILE_INODE_TYPE ? "FILE"
			: "UNKNOWN";
		auto create_time = ctime((time_t*)&inode.create_time);
		create_time[strlen(create_time) - 1] = '\0';
		printf("%7s %24s %8d %s\n", type_str, create_time, inode.size, entry.filename);
	}
}

void cmd::cp(const char* src, const char* des) {
	INode src_inode = getFileINode(selectINode(cwd_inode_num), src);
	if (src_inode.num == 0) {
		printf("Error: \"%s\" does not exist!\n", src);
		return;
	}
	if (src_inode.type != FILE_INODE_TYPE) {
		printf("Error: \"%s\" is not a file!\n", src);
		return;
	}
	if (countFreeBlocks() < (src_inode.size + BLOCK_SIZE - 1) / BLOCK_SIZE) {
		printf("Error: Not enough space!\n");
		return;
	}
	if (strlen(getFilename(des)) > FILENAME_MAX_LENGTH) {
		printf("Error: The filename is too long!\n");
		return;
	}
	if (!strcmp(getFilename(des), "..")) {
		printf("Error: \"..\" is not a valid filename!\n");
		return;
	}
	INode dir_inode = getDirINode(selectINode(cwd_inode_num), des);
	if (dir_inode.num == 0) {
		printf("Error: The path does not exist!\n");
		return;
	}
	if (getFileINode(selectINode(cwd_inode_num), des).num != 0) {
		printf("Error: \"%s\" already exists!\n", des);
		return;
	}
	INode des_inode = createFile(dir_inode, getFilename(des));
	copyFile(src_inode, des_inode);
}

void cmd::sum() {
	int cnt = countFreeBlocks();
	printf("Blocks  Used Available Use%%\n");
	printf("%6d %5d %9d %3d%%\n", BLOCK_NUMBER, BLOCK_NUMBER - cnt, cnt, ((BLOCK_NUMBER - cnt) * 100 + BLOCK_NUMBER / 2) / BLOCK_NUMBER);
}

void cmd::cat(const char *filename) {
	INode file_inode = getFileINode(selectINode(cwd_inode_num), filename);
	if (file_inode.num == 0) {
		printf("Error: \"%s\" does not exist!\n", filename);
		return;
	}
	if (file_inode.type != FILE_INODE_TYPE) {
		printf("Error: \"%s\" is not a file!\n", filename);
		return;
	}
	AddrBlock cache;
	for (int logic_addr = 0; logic_addr < file_inode.size; logic_addr += BLOCK_SIZE) {
		char block[BLOCK_SIZE];
		getBlock(file_inode.convertAddress(logic_addr, cache), block);
		for (int i = 0; i < BLOCK_SIZE && block[i] != '\0'; ++i)
			putchar(block[i]);
	}
	putchar('\n');
}

void commandInitializer() {
	srand(time(NULL));
	cwd_inode_num = ROOT_INODE_NUM;
}
