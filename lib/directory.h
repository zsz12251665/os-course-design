#ifndef _DIRCTORY_H
#define _DIRCTORY_H
#include <vector>
#include "inode.h"
#include "storage.h"

const int DIR_ENTRY_SIZE = 64;

struct DirEntry {
    int inode_number;
    char file_name[60];
    DirEntry(int, const char*);
};
typedef DirEntry DirBlock[16];

/*
create directory
1. create inode
2. find free data block
3. set size=1, dir_addr=foundblock, type=dir
4. return inode number
*/
INode createDir(int parent_inode_number); // 0


/*
delete directory
1. get data block from inode
2. free all the data blocks
3. free directory inode
4. return status
*/
bool deleteDir(const INode& inode);


/*
list entries in directory
1. get data block from inode
2. get entries from data blocks
3. return entries
*/
bool listDir(const INode& inode, std::vector<DirEntry>& entry_list);

/*
add an entry to directory file
1. get the last data block from inode
2. add entry to the data block
3. return status
*/
bool AddDirEntry(INode& dir_inode, const char* file_name, int inode_number);

/*
remove an entry to directory file
1. get the data block from inode
2. remove the entry and fill it with the last entry
3. return status
*/
bool RemoveDirEntry(INode& dir_inode, int inode_number);



#endif
