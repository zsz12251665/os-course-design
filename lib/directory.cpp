#include "directory.h"
#include "storage.h"
#include <cstring>

int getFirstBreakPosition(const char* path) {
	int len = strlen(path);
	for (int i = 0; i < len; ++i)
		if (path[i] == '/')
			return i;
	return len;
}

DirEntry::DirEntry() {
	memset(this, 0, sizeof this);
}

DirEntry::DirEntry(int inode_num, const char* filename) {
	int len = getFirstBreakPosition(filename);
	if (len > FILENAME_MAX_LENGTH)
		throw DirectoryError({"Filename is too long!", "DirEntry::DirEntry"});
	this->inode_num = inode_num;
	strncpy(this->filename, filename, len);
	this->filename[len] = '\0';
}

void addDirEntry(INode &dir_inode, int inode_num, const char *filename) {
	if (dir_inode.type != DIR_INODE_TYPE)
		throw DirectoryError({"dir_inode is not a directory!", "addDirEntry"});
	DirEntry entry(inode_num, filename);
	int block_num = dir_inode.size / BLOCK_SIZE, block_offset = dir_inode.size % BLOCK_SIZE;
	if (block_num >= 266) // 10 direct block + 256 indirect block
		throw DirectoryError({"dir_inode has reached its maximum size!", "addDirEntry"});
	int entry_addr = -1;
	if (block_num < 10) {
		if (dir_inode.direct_addr[block_num] == 0)
			dir_inode.direct_addr[block_num] = occupyBlock();
		entry_addr = dir_inode.direct_addr[block_num] + block_offset;
	} else {
		if (dir_inode.indirect_addr == 0)
			dir_inode.indirect_addr = occupyBlock();
		AddrBlock addr_block;
		getBlock(dir_inode.indirect_addr, addr_block);
		if (addr_block[block_num - 10] == 0) {
			addr_block[block_num - 10] = occupyBlock();
			putBlock(dir_inode.indirect_addr + (block_num - 10) * 4, &addr_block[block_num - 10], 4);
		}
		entry_addr = addr_block[block_num - 10] + block_offset;
	}
	putBlock(entry_addr, &entry, DIR_ENTRY_SIZE);
	dir_inode.size += DIR_ENTRY_SIZE;
	updateINode(dir_inode);
}

void removeDirEntry(INode &dir_inode, const char *filename) {
	if (dir_inode.type != DIR_INODE_TYPE)
		throw DirectoryError({"dir_inode is not a directory!", "removeDirEntry"});
	AddrBlock addr_block;
	addr_block[0] = 0;
	for (int logic_addr = 0; logic_addr < dir_inode.size; logic_addr += DIR_ENTRY_SIZE) {
		int entry_addr = dir_inode.convertAddress(logic_addr, addr_block);
		DirEntry entry;
		getBlock(entry_addr, &entry, DIR_ENTRY_SIZE);
		if (strcmp(filename, entry.filename) == 0) {
			if (logic_addr < dir_inode.size - DIR_ENTRY_SIZE) { // Not the last one
				int last_entry_addr = dir_inode.convertAddress(dir_inode.size - DIR_ENTRY_SIZE, addr_block);
				DirEntry last_entry;
				getBlock(last_entry_addr, &last_entry, DIR_ENTRY_SIZE);
				putBlock(entry_addr, &last_entry, DIR_ENTRY_SIZE); // Overwrite with the last one
			}
			dir_inode.size -= DIR_ENTRY_SIZE;
			break;
		}
	}
	// Trim the empty block
	if (dir_inode.size % BLOCK_SIZE == 0) {
		int block_cnt = dir_inode.size / BLOCK_SIZE;
		if (block_cnt < 10) {
			if (dir_inode.direct_addr[block_cnt] != 0) {
				freeBlock(dir_inode.direct_addr[block_cnt]);
				dir_inode.direct_addr[block_cnt] = 0;
			}
		} else {
			if (dir_inode.indirect_addr != 0) {
				if (addr_block[0] == 0)
					getBlock(dir_inode.indirect_addr, addr_block);
				if (addr_block[block_cnt - 10] != 0) {
					freeBlock(addr_block[block_cnt - 10]);
					if (block_cnt == 10) {
						freeBlock(dir_inode.indirect_addr);
						dir_inode.indirect_addr = 0;
					} else {
						addr_block[block_cnt - 10] = 0;
						putBlock(dir_inode.indirect_addr + (block_cnt - 10) * 4, &addr_block[block_cnt - 10], 4);
					}
				}
			}
		}
	}
	updateINode(dir_inode);
}

void initDir(INode &dir_inode, int parent_inode_num) {
	dir_inode.type = DIR_INODE_TYPE;
	addDirEntry(dir_inode, dir_inode.num, "."); // UpdateINode() included
	if (parent_inode_num != 0)
		addDirEntry(dir_inode, parent_inode_num, "..");
}
