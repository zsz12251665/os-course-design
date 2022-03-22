#ifndef _FILE_H
#define _FILE_H

#include "inode.h"

INode createFile(INode &dir_inode, const char *filename);
void deleteFile(INode &dir_inode, const char *filename);

void fillFile(INode &file_inode, int size); // size measured in bytes
void copyFile(const INode &src_inode, INode &des_inode);

INode getFileINode(const INode &cwd_inode, const char *path); // Path should not end with / unless it is root
INode getDirINode(const INode &cwd_inode, const char *path);

#endif // _FILE_H
