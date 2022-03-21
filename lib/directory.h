#ifndef _DIRECTORY_H
#define _DIRECTORY_H

#include "inode.h"

const int DIR_ENTRY_SIZE = 64; // sizeof DirEntry()
const int FILENAME_MAX_LENGTH = 59; // sizeof DirEntry().filename

struct DirectoryError {
	const char* message;
	const char* when;
};

struct DirEntry {
	int inode_num;
	char filename[FILENAME_MAX_LENGTH + 1];
	DirEntry();
	DirEntry(int inode_num, const char *filename);
};

typedef DirEntry DirBlock[16];

void addDirEntry(INode &dir_inode, int inode_num, const char *filename); // filename matches until the first break ('/' or '\0')
void removeDirEntry(INode &dir_inode, const char *filename); // filename matches exactly

void initDir(INode &dir_inode, int parent_inode_num = 0); // Ignore it when parent_inode_number == 0

#endif // _DIRECTORY_H
