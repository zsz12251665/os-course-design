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
    return file_inode;
}

bool fillFile(INode& file_inode, int size) {
    assert(size % BLOCK_SIZE == 0);
    file_inode.size = size;
    file_inode.type = FILE_INODE_TYPE;
    updateINode(file_inode);

    int data_addr, addr_addr;
    char random_data[BLOCK_SIZE];
    for (int i = 0; i < size / BLOCK_SIZE; ++i) {
        data_addr = findFreeBlock();
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
    INode file_inode = getFileINode(cwd, path);
    // free data block
    for (int i = 0; i < 10; ++i) {
        if (file_inode.direct_addr[i] != 0) {
            freeBlock(file_inode.direct_addr[i]);
        }
    }
    int pDes[BLOCK_SIZE / 4];
    if (file_inode.indirect_addr != 0 &&
        getBlock(file_inode.indirect_addr, pDes) == 0) {
        for (int i = 0; i < BLOCK_SIZE / 4; ++i) {
            if (pDes[i] != 0)
                freeBlock(pDes[i]);
        }
    }

    INode dir_inode = getDirINode(cwd, path);
    // delete dir entry
    RemoveDirEntry(dir_inode, file_inode.num);

    // free inode
    deleteINode(file_inode.num);

    return 0;
}

bool copyFile(const INode& src_inode, INode& des_inode) {
    int size = src_inode.size;
    des_inode.size = size;
    des_inode.type = src_inode.type;
    updateINode(des_inode);

    int data_addr, addr_addr;
    char src_data[BLOCK_SIZE];
    int pDes[BLOCK_SIZE / 4];
    if (size / BLOCK_SIZE >= 10) {
        getBlock(src_inode.indirect_addr, pDes);
    }

    for (int i = 0; i < size / BLOCK_SIZE; ++i) {
        data_addr = findFreeBlock();
        // read data from src
        if (i < 10) {
            getBlock(src_inode.direct_addr[i], src_data);
            putBlock(data_addr, src_data);
            des_inode.direct_addr[i] = data_addr;
        } else if (i == 10) {
            getBlock(pDes[i - 10], src_data);
            putBlock(data_addr, src_data);
            int addr_addr = findFreeBlock();
            putBlock(addr_addr, &data_addr, 4);
            des_inode.indirect_addr = addr_addr;
        } else {
            getBlock(pDes[i - 10], src_data);
            putBlock(data_addr, src_data);
            putBlock(des_inode.indirect_addr + 4 * (i - 10), &data_addr, 4);
        }
    }
    return 0;
}
