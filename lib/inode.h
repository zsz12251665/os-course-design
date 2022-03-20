#ifndef _INODE_H
#define _INODE_H

#include <ctime>

#define DIR_INODE_TYPE 0
#define FILE_INODE_TYPE 1

const int INODE_SIZE = 64; // sizeof INode()
const int INODE_MEMORY_START = 1 << 11;
const int INODE_MEMORY_END = 1 << 20;
const int INODE_MAX_NUMBER = (INODE_MEMORY_END - INODE_MEMORY_START) / INODE_SIZE;

struct INode
{
	int num; // Should be readonly
	int size;  // in Bytes
	unsigned long long create_time;
	int direct_addr[10];
	int indirect_addr;
	int type;
	INode();
};

const INode BLANK_INODE;

INode createINode();
INode selectINode(int num);
bool updateINode(const INode &inode);
bool deleteINode(int num);

void inodeInitializer();

#endif // _INODE_H
