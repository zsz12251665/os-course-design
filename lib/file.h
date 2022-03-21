#ifndef _FILE_H
#define _FILE_H
#include "inode.h"
#include "storage.h"

/* create an empty file
1. create file inode
2. add directory entry
*/
// INode createFile(INode dir_inode, const char* file_name);
INode createFile(INode cwd, const char* path);

// bool removeFile(INode dir_inode, const char* file_name);
bool removeFile(INode cwd, const char* path);

bool fillFile(INode& file_inode, int size); // in bytes

bool copyFile(const INode& src_inode, INode& des_inode);

INode getFileINode(INode cwd, const char* path);

INode getDirINode(INode cwd, const char* path);

#endif
