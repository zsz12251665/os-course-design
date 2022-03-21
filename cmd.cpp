#include "cmd.h"
#include "lib/storage.h"
#include "lib/inode.h"
#include "lib/directory.h"
#include "lib/file.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

INode cwd_inode;

const char* getFilename(const char* path) {
	int len = strlen(path);
	for (int i = len - 1; i >= 0; ++i)
		if (path[i] == '/')
			return path + i + 1;
	return path;
}

void cmd::createFile(const char* filename, int size) {
	if (countFreeBlock() < size) {
		printf("Error: Not enough space!\n");
		return;
	}
	INode file_inode = createFile(cwd_inode, filename);
	if (file_inode.num == 0) {
		printf("Error: \"%s\" cannot be created!\n", filename);
		return;
	}
	if (fillFile(file_inode, size)) {
		printf("Error: Fail to write data into \"%s\"!\n", filename);
		removeFile(cwd_inode, filename); //? Should have failure check?
	}
}

void cmd::deleteFile(const char* filename) {
	INode file_inode = getFileINode(cwd_inode, filename);
	if (file_inode.num == 0) {
		printf("Error: \"%s\" does not exist!\n", filename);
		return;
	}
	if (file_inode.type != FILE_INODE_TYPE) {
		printf("Error: \"%s\" is not a file!\n", filename);
		return;
	}
	if (removeFile(cwd_inode, filename))
		printf("Error: Fail to remove \"%s\"!\n", filename);
}

void cmd::createDir(const char* dirname) {
	; // TBC
}

void cmd::deleteDir(const char* dirname) {
	INode dir_inode = getFileINode(cwd_inode, dirname);
	if (dir_inode.num == 0) {
		printf("Error: \"%s\" does not exist!\n", dirname);
		return;
	}
	if (dir_inode.type != DIR_INODE_TYPE) {
		printf("Error: \"%s\" is not a directory!\n", dirname);
		return;
	}
	if (dir_inode.num == cwd_inode.num) {
		printf("Error: CWD could not be removed!\n");
		return;
	}
	if (dir_inode.num == ROOT_INODE.num) {
		printf("Error: / could not be removed!\n");
		return;
	}
	std::vector<DirEntry> entries;
	if (listDir(dir_inode, entries)) {
		printf("Error: Cannot list the entries in \"%s\"!\n", dirname);
		return;
	}
	if (entries.size() > 2) {
		printf("Error: \"%s\" is not empty!\n", dirname);
		return;
	}
	if (removeFile(cwd_inode, dirname))
		printf("Error: Fail to remove \"%s\"!\n", dirname);
}

void cmd::changeDir(const char* dirname) {
	INode res = getFileINode(cwd_inode, dirname);
	if (res.num == 0) {
		printf("Error: \"%s\" does not exist!\n", dirname);
		return;
	}
	if (res.type != DIR_INODE_TYPE) {
		printf("Error: \"%s\" is not a directory!\n", dirname);
		return;
	}
	cwd_inode = INode(res);
}

void cmd::dir() {
	std::vector<DirEntry> entries;
	if (listDir(cwd_inode, entries)) {
		printf("Error: Cannot list the entries in CWD!\n");
		return;
	}
	printf("   Type              Create Time  Size Filename\n");
	for (auto entry : entries) {
		INode inode = selectINode(entry.inode_number);
		if (inode.num == 0) {
			printf("Error: Fail to get the INode of \"%s\"", entry.file_name);
			return;
		}
		auto type_str = inode.type == DIR_INODE_TYPE ? "DIR"
			: inode.type == FILE_INODE_TYPE ? "FILE"
			: "UNKNOWN";
		printf("%7s %24s %5d %s\n", type_str, ctime((time_t*)&inode.create_time), inode.size, entry.file_name);
	}
}

void cmd::cp(const char* src, const char* des) {
	INode src_inode = getFileINode(cwd_inode, src);
	if (src_inode.num == 0) {
		printf("Error: \"%s\" does not exist!\n", src);
		return;
	}
	if (src_inode.type != FILE_INODE_TYPE) {
		printf("Error: \"%s\" is not a file!\n", src);
		return;
	}
	if (countFreeBlock() < (src_inode.size + BLOCK_SIZE - 1) / BLOCK_SIZE) {
		printf("Error: Not enough space!\n");
		return;
	}
	INode des_inode = createFile(cwd_inode, des);
	if (des_inode.num == 0) {
		printf("Error: \"%s\" cannot be created!\n", des);
		return;
	}
	if (copyFile(src_inode, des_inode)) {
		printf("Error: Fail to copy \"%s\" to \"%s\"!\n", src, des);
		removeFile(cwd_inode, des); //? Should have failure check?
	}
}

void cmd::sum() {
	int cnt = countFreeBlock();
	printf("Blocks  Used Available Use%% Mounted on\n");
	printf("%6d %5d %9d %3d%% %s\n", BLOCK_NUMBER, BLOCK_NUMBER - cnt, cnt, ((BLOCK_NUMBER - cnt) * 100 + BLOCK_NUMBER / 2) / BLOCK_NUMBER, "/");
}

void cmd::cat(const char *filename) {
	INode file_inode = getFileINode(cwd_inode, filename);
	if (file_inode.num == 0) {
		printf("Error: \"%s\" does not exist!\n", filename);
		return;
	}
	if (file_inode.type != FILE_INODE_TYPE) {
		printf("Error: \"%s\" is not a file!\n", filename);
		return;
	}
	int block_cnt = (file_inode.size + BLOCK_SIZE - 1) / BLOCK_SIZE;
	char block[BLOCK_SIZE];
	for (int i = 0; i < 10 && i < block_cnt; ++i) {
		if (getBlock(file_inode.direct_addr[i], block)) {
			printf("Error: A data block of the file is broken!\n");
			return;
		}
		for (int i = 0; i < BLOCK_SIZE && block[i] != '\0'; ++i)
			putchar(block[i]);
	}
	if (block_cnt > 10) {
		int addr_block[BLOCK_SIZE / 4];
		if (getBlock(file_inode.indirect_addr, addr_block)) {
			printf("Error: The indirect address block of the file is broken!\n");
			return;
		}
		for (int i = 0; i + 10 < block_cnt && i < BLOCK_SIZE / 4; ++i) {
			if (getBlock(addr_block[i], block)) {
				printf("Error: A data block of the file is broken!\n");
				return;
			}
			for (int i = 0; i < BLOCK_SIZE && block[i] != '\0'; ++i)
				putchar(block[i]);
		}
	}
	putchar('\n');
}

void commandInitializer() {
	cwd_inode = INode(ROOT_INODE);
	if (cwd_inode.num == 0) {
		printf("Error: Fail to initialize CWD!\n");
		exit(-1);
	}
}
