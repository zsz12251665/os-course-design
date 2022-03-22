#include "file.h"
#include "storage.h"
#include "directory.h"
#include <cstdlib>
#include <cstring>

INode createFile(INode &dir_inode, const char *filename) {
	INode file_inode = createINode();
	addDirEntry(dir_inode, file_inode.num, filename);
	return file_inode;
}

void deleteFile(INode &dir_inode, const char *filename) {
	INode file_inode = getFileINode(dir_inode, filename);
	int block_cnt = (file_inode.size + BLOCK_SIZE - 1) / BLOCK_SIZE;
	for (int i = 0; i < 10 && i < block_cnt; ++i)
		freeBlock(file_inode.direct_addr[i]);
	if (block_cnt > 10) {
		AddrBlock addr_block;
		getBlock(file_inode.indirect_addr, addr_block);
		for (int i = 10; i < block_cnt; ++i)
			freeBlock(addr_block[i - 10]);
		freeBlock(file_inode.indirect_addr);
	}
	deleteINode(file_inode.num);
	removeDirEntry(dir_inode, filename);
}

void fillFile(INode &file_inode, int size) {
	if (size > BLOCK_SIZE * 266) // 10 direct block + 256 indirect block
		throw DirectoryError({"The file is too big to fill!", "fillFile"});
	file_inode.type = FILE_INODE_TYPE;
	file_inode.size = size;
	char data_block[BLOCK_SIZE];
	AddrBlock addr_block;
	for (int i = 0; i * BLOCK_SIZE < size; ++i) {
		memset(data_block, 0, BLOCK_SIZE);
		for (int j = 0; j < size - i * BLOCK_SIZE && j < BLOCK_SIZE; ++j)
			data_block[j] = 32 + rand() % (127 - 32); // ASCII 32~126
		if (i < 10) {
			file_inode.direct_addr[i] = occupyBlock();
			putBlock(file_inode.direct_addr[i], data_block);
		} else {
			if (file_inode.indirect_addr == 0) {
				file_inode.indirect_addr = occupyBlock();
				memset(addr_block, 0, BLOCK_SIZE);
			}
			addr_block[i - 10] = occupyBlock();
			putBlock(addr_block[i - 10], data_block);
		}
	}
	if (file_inode.indirect_addr != 0)
		putBlock(file_inode.indirect_addr, addr_block);
	updateINode(file_inode);
}

void copyFile(const INode &src_inode, INode &des_inode) {
	des_inode.type = src_inode.type;
	des_inode.size = src_inode.size;
	char data_block[BLOCK_SIZE];
	for (int i = 0; i < 10 && i * BLOCK_SIZE < src_inode.size; ++i) {
		getBlock(src_inode.direct_addr[i], data_block);
		des_inode.direct_addr[i] = occupyBlock();
		putBlock(des_inode.direct_addr[i], data_block);
	}
	if (src_inode.size > 10 * BLOCK_SIZE) {
		AddrBlock addr_block;
		getBlock(src_inode.indirect_addr, addr_block);
		for (int i = 10; i * BLOCK_SIZE < src_inode.size; ++i) {
			getBlock(addr_block[i - 10], data_block);
			addr_block[i - 10] = occupyBlock();
			putBlock(addr_block[i - 10], data_block);
		}
		des_inode.indirect_addr = occupyBlock();
		putBlock(des_inode.indirect_addr, addr_block);
	}
	updateINode(des_inode);
}

INode getFileINode(const INode &cwd_inode, const char *path) {
	if (cwd_inode.type != DIR_INODE_TYPE)
		return BLANK_INODE;
    if (path[0] == '/') {
        return path[1] == '\0' ? ROOT_INODE : getFileINode(ROOT_INODE, path + 1);
    } else {
		AddrBlock cache;
		cache[0] = 0;
		for (int logic_addr = 0; logic_addr < cwd_inode.size; logic_addr += DIR_ENTRY_SIZE) {
			DirEntry entry;
			getBlock(cwd_inode.convertAddress(logic_addr, cache), &entry, DIR_ENTRY_SIZE);
			int len = strlen(entry.filename);
			if (strncmp(entry.filename, path, len) == 0) {
				if (path[len] == '\0')
					return selectINode(entry.inode_num);
				if (path[len] == '/')
					return getFileINode(selectINode(entry.inode_num), path + len + 1);
			}
		}
    }
	return BLANK_INODE;
}

INode getDirINode(const INode &cwd_inode, const char *path) {
	if (cwd_inode.type != DIR_INODE_TYPE)
		return BLANK_INODE;
	for (int i = 0, len = strlen(path); i <= len; ++i) {
		if (path[i] == '/')
			break;
		if (path[i] == '\0')
			return cwd_inode;
	}
    if (path[0] == '/') {
        return path[1] == '\0' ? BLANK_INODE : getDirINode(ROOT_INODE, path + 1);
    } else {
		AddrBlock cache;
		cache[0] = 0;
		for (int logic_addr = 0; logic_addr < cwd_inode.size; logic_addr += DIR_ENTRY_SIZE) {
			DirEntry entry;
			getBlock(cwd_inode.convertAddress(logic_addr, cache), &entry, DIR_ENTRY_SIZE);
			int len = strlen(entry.filename);
			if (strncmp(entry.filename, path, len) == 0) {
				if (path[len] == '/')
					return getDirINode(selectINode(entry.inode_num), path + len + 1);
			}
		}
    }
	return BLANK_INODE;
}
