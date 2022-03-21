#include "file.h"
#include "directory.h"
#include <assert.h>
#include <cstring>
#include <cstdlib>

int getNextEntry(const char* path) {
    for (int i = 0; i < strlen(path); ++i) {
        if (path[i] == '/') {
            return i;
        }
    }
    return strlen(path);
} 

const char* getFilename(const char* path) {
	int len = strlen(path);
	for (int i = len - 1; i >= 0; ++i)
		if (path[i] == '/')
			return path + i + 1;
	return path;
}


INode getFileINode(INode cwd, const char* path) {
    if (path[0] == '/') {
        return getFileINode(ROOT_INODE, path + 1);
    } else {
        char entry_name[256];
        strcpy(entry_name, path);
        int idx = getNextEntry(path);
        entry_name[idx] = '\0';
        INode entry_inode = getEntryINode(cwd, entry_name);
        if (idx == strlen(path)) {
            return entry_inode;
        }
        if(entry_inode.num == 0) 
            return BLANK_INODE;
        return getFileINode(entry_inode, path + idx + 1);
    }
}


INode getDirINode(INode cwd, const char* path) {
    char new_path[256];
    strcpy(new_path, path);
    for (int i = strlen(path) - 1; i >= 0; --i) {
        if (path[i] == '/') {
            new_path[i] == '\0';
            break;
        }
    }
    strcat(new_path, "/.");
    return getFileINode(cwd, new_path);
}

INode createFile(INode cwd, const char* path) {
    INode file_inode = createINode();
    INode dir_inode = getDirINode(cwd, path);
    AddDirEntry(dir_inode, getFilename(path), file_inode.num);
}

bool fillFile(INode& file_inode, int size) {
    assert(size % BLOCK_SIZE == 0);
    file_inode.size = size;
    file_inode.type = FILE_INODE_TYPE;
    updateINode(file_inode);
    
    int data_addr, addr_addr;
    for (int i = 0; i < size / BLOCK_SIZE; ++i) {
        data_addr = findFreeBlock();
        char random_data[BLOCK_SIZE];
        for(int j = 0; j < BLOCK_SIZE; ++j) {
            random_data[i] = char(rand() % (127 - 32) + 32); // 32 - 126
        }
        putBlock(data_addr, random_data);
        if (i < 10)
            file_inode.direct_addr[i] = data_addr;
        else if (i == 10) {
            int indirect_addr = findFreeBlock();
            putBlock(indirect_addr, &data_addr, 4);
            file_inode.indirect_addr = indirect_addr;
        } else {
            addr_addr = findFreeBlock();
            putBlock(file_inode.indirect_addr + 4 * (i - 10), &addr_addr, 4);
        }
    }
    return 0;
}

bool removeFile(INode cwd, const char* path) {

}

bool copyFile(const INode& src_inode, INode& des_inode) {

}



