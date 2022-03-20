#ifndef _FILE_H
#define _FILE_H
#include "inode.h"
#include "storage.h"
#include "directory.h"

// create empty file
INode createFile(INode dir_inode, const char* file_name);

bool removeFile(INode dir_inode, const char* file_name);

bool fillFile(INode& file_inode, int size); // in bytes

bool copyFile(const INode& src_inode, INode& des_inode);

INode getFileINode(INode cwd, const char* path);



#endif
