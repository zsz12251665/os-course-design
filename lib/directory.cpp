#include "directory.h"
#include <cstring>

DirEntry::DirEntry(int inode_number, const char* file_name) {
    this->inode_number = inode_number;
    strcpy(this->file_name, file_name);
}

INode createDir(int parent_inode_number) {
    INode dir = createINode();
    int data_addr = findFreeBlock();
    if (data_addr == 0) 
        return dir;
    dir.size = 128;
    dir.direct_addr[0] = data_addr;
    dir.type = DIR_INODE_TYPE;

    // write . and .. into directory table
    DirEntry cur(dir.num, ".");
    DirEntry par(parent_inode_number, "..");
    putBlock(data_addr, &cur, DIR_ENTRY_SIZE);
    putBlock(data_addr + DIR_ENTRY_SIZE, &par, DIR_ENTRY_SIZE);

    return dir;
}

bool deleteDir(const INode& inode) {
    //! only empty directory is allowed to delete


    // free data block
    for (int i = 0; i < 10; ++i) {
        if (inode.direct_addr[i] != 0) {
            freeBlock(inode.direct_addr[i]);
        }
    }
    int pDes[BLOCK_SIZE / 4];
    if (inode.indirect_addr != 0 && 
        getBlock(inode.indirect_addr, pDes) == 0) {
        for (int i = 0; i < BLOCK_SIZE / 4; ++i) {
            if (pDes[i] != 0)
                freeBlock(pDes[i]);
        }
    }

    // free inode
    deleteINode(inode.num);
    return 0;
}

bool listDir(const INode& inode, std::vector<DirEntry>& entry_list) {
    char pData[BLOCK_SIZE];
    for (int i = 0; i < 10; ++i) {
        if (inode.direct_addr[i] != 0) {
            getBlock(inode.direct_addr[i], pData);
            for (int j = 0; j < BLOCK_SIZE; j += DIR_ENTRY_SIZE) {
                entry_list.push_back(*(DirEntry*)(pData + j));
            }
        }
    }
    int pAddr[BLOCK_SIZE / 4];
    if (inode.indirect_addr != 0 && 
        getBlock(inode.indirect_addr, pAddr) == 0) {
        for (int i = 0; i < BLOCK_SIZE / 4; ++i) {
            if (pAddr[i] != 0) {
                getBlock(pAddr[i], pData);
                for (int j = 0; j < BLOCK_SIZE; j += DIR_ENTRY_SIZE) {
                    entry_list.push_back(*(DirEntry*)(pData + j));
                }
            }
        }
    }
    return 0;
}

bool AddDirEntry(INode& dir_inode, char* file_name, int inode_number) {
    dir_inode.size += DIR_ENTRY_SIZE;
    DirEntry entry(inode_number, file_name);
    int cur_addr = getLastFreeEntry(dir_inode);
    // put the entry on the first free addr.
    putBlock(cur_addr, &entry, DIR_ENTRY_SIZE);
    return 0;
}

int getLastFreeEntry(const INode& dir_inode) {
    int cur_block = dir_inode.size / BLOCK_SIZE;
    int cur_addr;
    if (cur_block < 10) {
        cur_addr = dir_inode.direct_addr[cur_block] 
                        + (dir_inode.size - cur_block * BLOCK_SIZE);
    } else {
        int pDes[BLOCK_SIZE / 4];
        if (getBlock(dir_inode.indirect_addr, pDes) == 0) {
            cur_addr = pDes[cur_block - 10] 
                            + (dir_inode.size - cur_block * BLOCK_SIZE);
        }   
    }
    return cur_addr;
}

bool RemoveDirEntry(INode& dir_inode, int inode_number) {
    dir_inode.size -= DIR_ENTRY_SIZE;
    int cur_addr = getLastFreeEntry(dir_inode);
    char pDes[DIR_ENTRY_SIZE];
    getBlock(cur_addr, pDes, DIR_ENTRY_SIZE);

    char pData[BLOCK_SIZE];
    for (int i = 0; i < 10; ++i) {
        if (dir_inode.direct_addr[i] != 0) {
            getBlock(dir_inode.direct_addr[i], pData);
            for (int j = 0; j < BLOCK_SIZE; j += DIR_ENTRY_SIZE) {
                DirEntry entry = *(DirEntry*)(pData + j);
                if (entry.inode_number == inode_number) {
                    putBlock(dir_inode.direct_addr[i] + j, pDes, DIR_ENTRY_SIZE);
                    return 0;
                }
            }
        }
    }
    int pAddr[BLOCK_SIZE / 4];
    if (dir_inode.indirect_addr != 0 && 
        getBlock(dir_inode.indirect_addr, pAddr) == 0) {
        for (int i = 0; i < BLOCK_SIZE / 4; ++i) {
            if (pAddr[i] != 0) {
                getBlock(pAddr[i], pData);
                for (int j = 0; j < BLOCK_SIZE; j += DIR_ENTRY_SIZE) {
                    DirEntry entry = *(DirEntry*)(pData + j);
                    if (entry.inode_number == inode_number) {
                        putBlock(pAddr[i] + j, pDes, DIR_ENTRY_SIZE);
                        return 0;
                    }
                }
            }
        }
    }

    return 1; // can not find such a inode
}
